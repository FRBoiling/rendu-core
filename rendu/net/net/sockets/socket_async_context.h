/*
* Created by boil on 2024/3/18.
*/

#ifndef RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_CONTEXT_H_
#define RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_CONTEXT_H_

#include "safe_socket_handle.h"
#include "socket_error.h"

NET_NAMESPACE_BEGIN

class SocketAsyncContext {

  enum class OperationResult {
    Pending = 0,
    Completed = 1,
    Cancelled = 2
  };

  class AsyncOperation : public IThreadPoolWorkItem {
  public:
    AsyncOperation(SocketAsyncContext *context) {
      m_associated_context = context;
      Reset();
    }

    void Reset() {
      _state = (int) State::Waiting;
      //    Event = null;
      m_next = this;
    }

    void DoAbort() {
      m_error_code = SocketError::OperationAborted;
    }

    OperationResult TryComplete(SocketAsyncContext *context) {
      int expected_state = (int) State::Waiting;
      int newState = (int) State::Running;
      // 尝试将_state的值设置为new_value（State.Running）
      // 同时保存_state原始的值到expected_value中。
      // 如果_state的原始值等于 our expected_value（即State.Waiting），那么compare_exchange_strong会返回true
      // 如果_state原始的值不等于我们的expected_value，那么方法会返回false，并更新expected_value为_state当前的值
       _state.compare_exchange_strong(expected_state, newState);
      int oldState = expected_state;// expected_value holds the old value of _state regardless of whether the exchange happened or not
      if (oldState == (int) State::Canceled) {
        return OperationResult::Cancelled;
      }

      if (DoTryComplete(context)) {

        return OperationResult::Completed;
      }

      while (true) {
        expected_state = _state.load();

        newState = (expected_state == (int) State::Running ? (int) State::Waiting : (int) State::Canceled);

        _state.compare_exchange_strong(expected_state, newState);
        oldState = expected_state;// expected_value holds the old value of _state regardless of whether the exchange happened or not

        if (expected_state == oldState) {
          break;
        }
        // Race to update the state. Loop and try again.
      }

      if (newState == (int) State::Canceled) {
        ProcessCancellation();
        RD_TRACE("Exit, Newly cancelled");
        return OperationResult::Cancelled;
      }

      RD_TRACE("Exit, Pending");
      return OperationResult::Pending;
    }


    void ProcessCancellation(){
      m_error_code = SocketError::OperationAborted;
      //TODO:BOIL

    }

  protected:
    virtual bool DoTryComplete(SocketAsyncContext *context) = 0;


  public:
    Memory<byte> *m_socket_address;
    SocketError m_error_code;
    AsyncOperation *m_next;
    SocketAsyncContext *m_associated_context;
    CancellationTokenRegistration *m_cancellation_registration;


  private:
    enum State {
      Waiting = 0,
      Running,
      RunningWithPendingCancellation,
      Complete,
      Canceled
    };

    std::atomic<int> _state;// Actually AsyncOperation.State.
  };

  class ReadOperation : public AsyncOperation {

  public:
    ReadOperation(SocketAsyncContext *context) : AsyncOperation(context) {}

  public:
    void Execute() override {
      //TODO:BOIL
      //    return AssociatedContext.ProcessAsyncReadOperation(this);
      return;
    }
  };

  class WriteOperation : public AsyncOperation {
  public:
    WriteOperation(SocketAsyncContext *context) : AsyncOperation(context) {}

  public:
    void Execute() override {
      //TODO:BOIL
      //    return AssociatedContext.ProcessAsyncReadOperation(this);
      return;
    }
  };


  class AcceptOperation : public ReadOperation {
  public:
    AcceptOperation(SocketAsyncContext *context) : ReadOperation(context) {}

  public:
    using CallBack = std::function<void(int *, Memory<byte> *, SocketError &)>;

  protected:
    bool DoTryComplete(SocketAsyncContext *context) override;


  public:
    int m_accepted_fd;
    CallBack m_callback;
  };

  class ConnectOperation : public WriteOperation {
  };

  class SendFileOperation : public WriteOperation {
  };


public:
  SocketError AcceptAsync(Memory<byte> *socketAddress, int &socketAddressLen, int &acceptedFd, AcceptOperation::CallBack callback, CancellationToken cancellationToken);
  SocketError ConnectAsync(Memory<byte> socketAddress, int socketAddressLen, int &socket, std::function<void(int, SocketError &)> callback, CancellationToken cancellationToken);

  void SetHandleNonBlocking();

  bool TryRegister(interop::Error &error) {
    //TODO:BOIL
    return true;
  }
  bool IsRegistered() {
    //TODO:BOIL
    return false;
  }

private:
  void ReturnOperation(AcceptOperation &operation);


  template<typename TOperation>
  struct OperationQueue {
  public:
  private:
    enum QueueState : int {
      Ready = 0,     // Indicates that data MAY be available on the socket.
                     // Queue must be empty.
      Waiting = 1,   // Indicates that data is definitely not available on the socket.
                     // Queue must not be empty.
      Processing = 2,// Indicates that a thread pool item has been scheduled (and may
                     // be executing) to process the IO operations in the queue.
                     // Queue must not be empty.
      Stopped = 3,   // Indicates that the queue has been stopped because the
                     // socket has been closed.
                     // Queue must be empty.
    };

    std::atomic<QueueState> _state;// See above
    bool _isNextOperationSynchronous;
    std::atomic<int> _sequenceNumber;
    AsyncOperation *_tail;// Queue of pending IO operations to process when data becomes available.

  public:
    bool IsReady(SocketAsyncContext *context, int &observedSequenceNumber) {
      static_assert(sizeof(QueueState) == sizeof(int), "Size of QueueState is not equal to int");

      QueueState state = _state.load(std::memory_order_relaxed);               // similar to Volatile.Read
      observedSequenceNumber = _sequenceNumber.load(std::memory_order_relaxed);// similar to Volatile.Read

      bool isReady = state == QueueState::Ready || state == QueueState::Stopped;
      if (!isReady) {
        observedSequenceNumber--;
      }
      // Assuming you have some kind of printf/trace function
      RD_TRACE("IsReady {}", isReady ? "true" : "false");
      return isReady;
    }

    bool StartAsyncOperation(SocketAsyncContext *context, TOperation *operation, int observedSequenceNumber, CancellationToken cancellationToken) {

      RD_TRACE("enter")
      interop::Error error;
      if (!context->IsRegistered() && !context->TryRegister(error)) {
        HandleFailedRegistration(context, operation, error);

        std::cout << "Leave, not registered" << std::endl;
        ;
        return false;
      }

      while (true) {
        bool doAbort = false;
        {
          switch (_state) {
            case QueueState::Ready:
              if (observedSequenceNumber != _sequenceNumber) {
                // The queue has become ready again since we previously checked it.
                // So, we need to retry the operation before we enqueue it.
                assert(observedSequenceNumber - _sequenceNumber < 10000);
                observedSequenceNumber = _sequenceNumber;
                break;
              }

              // Caller tried the operation and got an EWOULDBLOCK, so we need to transition.
              _state = QueueState::Waiting;
              // -fallthrough!

            case QueueState::Waiting:
            case QueueState::Processing:
              // Enqueue the operation.
              assert(operation->m_next == operation);

              if (_tail == nullptr) {
                assert(!_isNextOperationSynchronous);
                _isNextOperationSynchronous = operation->m_next != nullptr;
              } else {
                operation->m_next = _tail->m_next;
                _tail->m_next = operation;
              }

              _tail = operation;

              //              std::cout << "Leave, enqueued " << std::to_string(IdOf(operation)) << std::endl;
              return true;
              ;

            case QueueState::Stopped:
              assert(_tail == nullptr);
              doAbort = true;
              break;

            default:
              std::cerr << "unexpected queue state" << std::endl;
              exit(EXIT_FAILURE);
              break;
          }
        }

        if (doAbort) {
          operation->DoAbort();
          std::cout << "Leave, queue stopped" << std::endl;
          return false;
        }

        // Retry the operation.
        if (operation->TryComplete(context) != OperationResult::Pending) {
          std::cout << "Leave, retry succeeded" << std::endl;
          return false;
        }
      }
    }

    static void HandleFailedRegistration(SocketAsyncContext *context, TOperation *operation, interop::Error error) {
      if (error == interop::Error::RD_EPIPE) {
        if (operation->TryComplete(context) != OperationResult::Pending) {
          return;
        }
      }

      if (error == interop::Error::RD_ENOMEM || error == interop::Error::RD_ENOSPC) {
        throw std::bad_alloc();
      } else {
        throw std::runtime_error("Internal error: " + std::to_string(error));
      }
    }
  };

public:
  SafeSocketHandle::Ptr m_socket;
  OperationQueue<ReadOperation> m_receive_queue;
  OperationQueue<WriteOperation> m_send_queue;

  //  TODO:BOIL
  //  SocketAsyncEngine * _asyncEngine;

private:
  bool m_is_handle_non_blocking;
  std::atomic<AsyncOperation *> m_cached_accept_operation;
};

NET_NAMESPACE_END

#endif//RENDU_NET_NET_SOCKETS_SOCKET_ASYNC_CONTEXT_H_
