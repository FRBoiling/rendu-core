/*
* Created by boil on 2022/10/23.
*/

#ifndef RENDU_ENGINE_H_
#define RENDU_ENGINE_H_

#include <events.h>
#include <log.h>
#include <platform/platform.h>
#include <platform/defines.h>
#include <platform/assert.h>
#include <types/vector_any.h>
#include <types/vector_unique.h>
#include <types/location_string.h>
#include <types/mutex.h>

#include <algorithm>
#include <cstring>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include "traits/cacheline.h"

namespace rendu
{
  class Engine final
  {
    //! Container key generator
    template <std::size_t Value>
    struct IUniqueKey {};

    //! Unique key for storage events type index
    using UKEventStorage = IUniqueKey<0>;

    //! Unique key for storage systems type index
    using UKSystems = IUniqueKey<1>;

    //! Event callback storage with type erasure
    struct CallbackStorage
    {
      using Function = void (*)();
      using MemberFunction = void (CallbackStorage::*)();
      union alignas(16) Storage {
        Function m_Callback;
        MemberFunction m_CallbackMember;
      };
      using Callback = void (*)(Storage&, void*);

      template <typename Ret, typename... Args>
      CallbackStorage(Ret (*callback)(Args...), Callback cb) : m_Callback{cb} {
        new (&m_Storage) decltype(callback){callback};
      }

      template <typename Ret, typename T, typename... Args>
      CallbackStorage(Ret (T::*callback)(Args...), Callback cb) : m_Callback{cb} {
        new (&m_Storage) decltype(callback){callback};
      }

      CallbackStorage(const CallbackStorage& rhs) = default;
      CallbackStorage(CallbackStorage&& rhs) noexcept = default;
      CallbackStorage& operator=(const CallbackStorage& rhs) = default;
      CallbackStorage& operator=(CallbackStorage&& rhs) noexcept = default;

      template <typename Ret, typename... Args>
      CallbackStorage& operator=(Ret (*callback)(Args...)) noexcept {
        new (&m_Storage) decltype(callback){callback};
        return *this;
      }

      template <typename Ret, typename T, typename... Args>
      CallbackStorage& operator=(Ret (T::*callback)(Args...)) noexcept {
        new (&m_Storage) decltype(callback){callback};
        return *this;
      }

      template <typename Ret, typename... Args>
      [[nodiscard]] bool operator==(Ret (*callback)(Args...)) const noexcept {
        decltype(callback) fn{}; std::memcpy(&fn, &m_Storage, sizeof(callback));
        return fn == callback;
      }

      template <typename Ret, typename T, typename... Args>
      [[nodiscard]] bool operator==(Ret (T::*callback)(Args...)) const noexcept {
        decltype(callback) fn{}; std::memcpy(&fn, &m_Storage, sizeof(callback));
        return fn == callback;
      }

      template <typename Ret, typename... Args>
      [[nodiscard]] bool operator!=(Ret (*callback)(Args...)) const noexcept {
        return !(*this == callback);
      }

      template <typename Ret, typename T, typename... Args>
      [[nodiscard]] bool operator!=(Ret (T::*callback)(Args...)) const noexcept {
        return !(*this == callback);
      }

      Storage m_Storage;
      Callback m_Callback;
    };

  public:
    //! Engine states
    enum class EState : std::uint8_t
    {
      Undefined,
      Init,
      Shutdown
    };

    //! Context for storage framework data
    class Context
    {
      friend class Engine;

      struct ShutdownMessage {
        std::string m_Message;
        types::SourceLocation m_Location;
        types::Mutex m_Mutex;
      };

      static constexpr auto DefaultTickrate = 1.f / 30.f;

    protected:
      using Callback = std::function<void ()>;

      template <typename T = Context>
      static T& GetInstance() noexcept {
        RENDU_ASSERT(m_Context, "Context not initilized");
        return *static_cast<T*>(m_Context.get());
      }

    public:
      Context() noexcept
          : m_Systems{}
          , m_Events{}
          , m_Callback{}
          , m_ShutdownMessage{}
          , m_ApplicationName{}
          , m_Tickrate{DefaultTickrate}
          , m_DeltaTime{}
          , m_TimeElapsed{}
          , m_TimeStart{}
          , m_TimeNow{}
          , m_TimePrev{}
          , m_State{EState::Undefined} {}
      ~Context() {
        m_Events.Clear();
        m_Systems.Clear();
      }
      Context(const Context&) = delete;
      Context(Context&&) noexcept = delete;
      Context& operator=(const Context&) = delete;
      Context& operator=(Context&&) noexcept = delete;

      /**
      * @brief Initialize context of Engine
      * @tparam T Context type
      * @tparam Args types of arguments used to construct
      * @param args arguments for context initialization
      * @note The context can be inherited
      */
      template <typename T = Context, typename... Args>
      requires std::is_base_of_v<Context, T> && std::is_constructible_v<T, Args...>
      static void Initialize([[maybe_unused]] Args&&... args) {
        RENDU_ASSERT(!m_Context, "Context already initialized!");
        m_Context = std::make_shared<T>(std::forward<Args>(args)...);
      }

      /**
      * @brief Initialize the engine context for sharing between the executable and plugins
      * @tparam T Context type
      * @param ctx Context object for support shared memory and across boundary
      * @note This overload is used to share the context object between the executable and plugins
      */
      template <typename T = Context>
      requires std::is_base_of_v<Context, T>
      static void Initialize(const std::shared_ptr<T>& ctx) noexcept {
        RENDU_ASSERT(ctx, "Context is empty!");
        RENDU_ASSERT(!m_Context || (ctx && m_Context == ctx), "Context already initialized!");
        m_Context = ctx;
      }

      /**
      * @brief Return a context object
      * @tparam T Context type
      * @return Return a shared pointer to a context object
      */
      template <typename T = Context>
      requires std::is_base_of_v<Context, T>
      [[nodiscard]] static std::shared_ptr<T> Get() noexcept {
        RENDU_ASSERT(m_Context, "Context not initialized");
        return std::static_pointer_cast<T>(m_Context);
      }

      /**
      * @brief Set the application name
      * @param name Application name
      * @note You can set any name and use it in your code
      */
      static void SetAppName(std::string name) noexcept {
        auto& ctx = GetInstance();
        ctx.m_ApplicationName = std::move(name);
      }

      /**
      * @brief Set the update tickrate for the engine "Update" event
      * @param tickrate Update frequency
      * @note By default, 30 frames per second
      */
      static void SetTickrate(float tickrate) noexcept {
        auto& ctx = GetInstance();
        ctx.m_Tickrate = 1.f / (std::max)(tickrate, 1.f);
      }

      /**
      * @brief Set the entry point for the engine
      * @param Callback Callback function
      * @note
      * Use the entry point to work with the framework
      * After calling the entry point, engine events are called for listeners
      */
      static void SetMain(Callback callback) noexcept {
        auto& ctx = GetInstance();
        ctx.m_Callback = std::move(callback);
      }

      /**
      * @brief Return the application name
      * @return String view object to the application name
      */
      [[nodiscard]] static std::string_view GetAppName() noexcept {
        const auto& ctx = GetInstance();
        return ctx.m_ApplicationName;
      }

      /**
      * @brief Returns the current engine tickrate
      * @return Tickrate in float
      */
      [[nodiscard]] static float GetTickrate() noexcept {
        const auto& ctx = GetInstance();
        return ctx.m_Tickrate;
      }

    private:
      types::VectorAny<UKSystems, traits::Cacheline> m_Systems;
      types::VectorUnique<UKEventStorage, std::vector<CallbackStorage>> m_Events;

      Callback m_Callback;

      ShutdownMessage m_ShutdownMessage;
      std::string m_ApplicationName;

      float m_Tickrate;
      float m_DeltaTime;
      float m_TimeElapsed;

      std::uint64_t m_TimeStart;
      std::uint64_t m_TimeNow;
      std::uint64_t m_TimePrev;

      std::atomic<Engine::EState> m_State;

      inline static std::shared_ptr<Context> m_Context;
    };

  private:
#if defined(RENDU_PLATFORM_WIN)
    static BOOL WINAPI CtrlHandler([[maybe_unused]] DWORD dwCtrlType);
        static LONG WINAPI MiniDumpSEH(EXCEPTION_POINTERS* pException);

#elif defined(RENDU_PLATFORM_LINUX)
    static void SigHandler([[maybe_unused]] int signal);
#endif

    static void RegisterHandlers();

  public:
    /**
    * @brief Heartbeat of the engine
    *
    * @code{.cpp}
    * while(rendu::Engine::Heartbeat()) {}
    * @endcode
    *
    * @return True if successful or false if an error is detected or called shutdown
    * @note
    * You have to call heartbeat in a loop to keep the framework running
    * Use the definition of RENDU_ENGINE_NO SLEEP to prevent sleep by 1 ms
    * The thread will not sleep if your operations consume a lot of CPU time
    */
    [[nodiscard]] static bool Heartbeat();

    /**
    * @brief Check if the engine is currently running
    * @return True if running, false if shutdown or not initialized
    * @note This function is similar to calling GetState() == EState::Init;
    */
    [[nodiscard]] static bool Running() noexcept;

    /**
    * @brief Return the current state of the engine
    * @return EState state flag
    */
    [[nodiscard]] static EState GetState() noexcept;

    /**
    * @brief Shutdown the engine (thread safe)
    *
    * @code{.cpp}
    * rendu::Engine::Shutdown(); // no error
    * rendu::Engine::Shutdown("Unknown error");
    * rendu::Engine::Shutdown("Error code: {}, text: {}", 55, "examples");
    * @endcode
    *
    * @tparam Args types of arguments
    * @param msg Reason message
    * @param args arguments for formatting the message
    * @note If the error message is not empty, an error message will be displayed in the console
    * @warning If there is no console, the message will not be displayed
    */
    template <typename... Args>
    static void Shutdown(const types::LocationString& msg = {}, [[maybe_unused]] Args&&... args);

    /**
    * @brief Returns the last shutdown reason
    *
    * @return Reason string if the engine State == EState::Shutdown and has a reason, empty otherwise
    * @warning No point in calling this function if State != EState::Shutdown
    */
    [[nodiscard]] static auto ShutdownReason() noexcept;

    /**
    * @brief Register the system in the engine
    *
    * @code{.cpp}
    * struct MySystem {};
    * rendu::Engine::RegisterSystem<MySystem>();
    * @endcode
    *
    * @tparam T Type of system
    * @tparam Args types of arguments
    * @param args arguments for system initialization
    */
    template <typename T, typename... Args>
    static void RegisterSystem([[maybe_unused]] Args&&... args);

    /**
    * @brief Check the exist of system
    *
    * @code{.cpp}
    * struct MySystemA{};
    * struct MySystemB{};
    *
    * rendu::Engine::RegisterSystem<MySystemA, MySystemB>();
    * if(rendu::Engine::HasSystem<MySystemA, MySystemB>()) {
    *   // ok
    * }
    * @endcode
    *
    * @tparam T types of systems
    * @return True if all types of systems exist, or false
    */
    template <typename... T>
    [[nodiscard]] static bool HasSystem();

    /**
    * @brief Check the exist of any system
    *
    * @code{.cpp}
    * struct MySystemA{};
    * struct MySystemB{};
    *
    * rendu::Engine::RegisterSystem<MySystemA>();
    * if(rendu::Engine::AnySystem<MySystemA, MySystemB>()) {
    *   // ok
    * }
    * @endcode
    *
    * @tparam T types of systems
    * @return True if any types of systems exist, or false
    */
    template <typename... T>
    [[nodiscard]] static bool AnySystem();

    /**
    * @brief Get a reference to the system
    *
    * @code{.cpp}
    * struct MySystemA{};
    * struct MySystemB{};
    *
    * rendu::Engine::RegisterSystem<MySystemA>();
    * rendu::Engine::RegisterSystem<MySystemB>();
    *
    * const auto& [systemA, systemB] = rendu::Engine::GetSystem<MySystemA, MySystemB>();
    * @endcode
    *
    * @tparam T types of systems
    * @return Reference to a system or tuple if multiple types of systems are passed
    */
    template <typename... T>
    [[nodiscard]] static decltype(auto) GetSystem();

    /**
    * @brief Remove the system from engine
    *
    * @code{.cpp}
    * struct MySystemA{};
    * struct MySystemB{};
    *
    * rendu::Engine::RegisterSystem<MySystemA>();
    * rendu::Engine::RegisterSystem<MySystemB>();
    *
    * rendu::Engine::RemoveSystem<MySystemA, MySystemB>();
    * @endcode
    *
    * @tparam T types of systems
    */
    template <typename... T>
    static void RemoveSystem();

    /**
    * @brief Listening to the event
    *
    * @code{.cpp}
    * void OnInit() {
    *   // The event is called when the engine is initialized
    * }
    *
    * rendu::Engine::SubscribeEvent<rendu::Events::Engine::Init>(&OnInit);
    * @endcode
    *
    * @tparam Event Type of event
    * @tparam Args types of arguments
    * @param callback Callback function
    */
    template <typename Event, typename... Args>
    static void SubscribeEvent(void (*callback)([[maybe_unused]] Args...));

    /**
    * @brief Listening to the event
    *
    * @code{.cpp}
    * struct MySystem {
    *   void OnInit() {
    *       // The event is called when the engine is initialized
    *   }
    * };
    * rendu::Engine::SubscribeEvent<rendu::Events::Engine::Init>(&MySystem::OnInit);
    * @endcode
    *
    * @tparam Event Type of event
    * @tparam System Type of system
    * @tparam Args types of events
    * @param callback Callback function
    */
    template <typename Event, typename System, typename... Args>
    static void SubscribeEvent(void (System::*callback)([[maybe_unused]] Args...));

    /**
    * @brief Trigger an event for all listeners
    *
    * @code{.cpp}
    * struct MySystem {
    *   void OnInit() {
    *       // The event is called when the engine is initialized
    *   }
    * };
    * rendu::Engine::SubscribeEvent<rendu::Events::Engine::Init>(&MySystem::OnInit);
    * @endcode
    *
    * @tparam Event Type of event
    * @tparam Args types of arguments
    * @param args arguments for construct the event
    */
    template <typename Event, typename... Args>
    static void SignalEvent([[maybe_unused]] Args&&... args);

    /**
    * @brief Stop listening to the event
    *
    * @code{.cpp}
    * struct MySystem {
    *   void OnInit() {
    *       // The event is called when the engine is initialized
    *   }
    * };
    *
    * rendu::Engine::UnsubscribeEvent<rendu::Events::Engine::Init>(&OnInit);
    * @endcode
    *
    * @tparam Event Type of event
    * @tparam Args types of arguments
    * @param callback Callback function
    */
    template <typename Event, typename... Args>
    static void UnsubscribeEvent(void (*callback)([[maybe_unused]] Args...));

    /**
    * @brief Stop listening to the event
    *
    * @code{.cpp}
    * struct MySystem {
    *   void OnInit() {
    *       // The event is called when the engine is initialized
    *   }
    * };
    *
    * rendu::Engine::UnsubscribeEvent<rendu::Events::Engine::Init>(&MySystem::OnInit);
    * @endcode
    *
    * @tparam Event Type of event
    * @tparam System Type of system
    * @tparam Args types of arguments
    * @param callback Callback function
    */
    template <typename Event, typename System, typename... Args>
    static void UnsubscribeEvent(void (System::*callback)([[maybe_unused]] Args...));
  };

} //rendu


#endif //RENDU_ENGINE_H_
