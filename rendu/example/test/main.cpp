/* For POSIX systems we use the standard BSD socket API. */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifdef __APPLE__
#include <sys/event.h>
#endif
#include <unistd.h>
#include <cstddef>
#include <cstring>
#include <string>
#include <iostream>


int BindSocket() {
  int fd;
  /* Create socket */
  fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket error");
    return -1;
  }
  struct sockaddr_in6 addr;
  /* Clear address struct */
  memset(&addr, 0, sizeof(sockaddr));
  /* Set address family to Unix domain sockets */
  addr.sin6_family = AF_INET6;
  addr.sin6_port = htons(63456);
  addr.sin6_addr = IN6ADDR_ANY_INIT;
  /* Bind socket to address */
  if (::bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(sockaddr_in6)) == -1) {
    perror("bind error");
    close(fd);
    return -1;
  }
  return fd;
}


int BindIpv4Socket() {
  int fd;
  /* Create socket */
  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket error");
    return -1;
  }
  struct sockaddr_in addr;
  /* Clear address struct */
  memset(&addr, 0, sizeof(sockaddr));
  /* Set address family to Unix domain sockets */
  addr.sin_family = AF_INET;
  addr.sin_port = htons(23456);
  addr.sin_addr.s_addr = INADDR_ANY;
  /* Bind socket to address */
  if (::bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
    perror("bind error");
    close(fd);
    return -1;
  }
  return fd;
}



int BindUnixSocket() {
  struct sockaddr_un addr_un;
  int fd;
  /* Create socket */
  fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (fd == -1) {
    perror("socket error");
    return -1;
  }
  /* Clear address struct */
  memset(&addr_un, 0, sizeof(sockaddr_un));

  /* Set address family to Unix domain sockets */
  addr_un.sun_family = AF_UNIX;

  /* Set address */
  std::string unix_path = "/tmp/my_socket";
  /* Ensure the socket file does not exist before binding */
  unlink(unix_path.c_str()); // Ignore errors, as file may not exist

  strncpy(addr_un.sun_path, unix_path.c_str(), sizeof(addr_un.sun_path) - 1);

  /* Bind socket to address */
  if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr_un), sizeof(sockaddr_un)) == -1) {
    perror("bind error");
    close(fd);
    return -1;
  }
  return fd;
}

int GetSockName1111(int socket, std::byte *sock_addr, int &addr_len) {
  sockaddr &tmp_addr = (struct sockaddr &)*sock_addr;
  socklen_t tmp_addr_len = sizeof(sockaddr);
  int raw_error = ::getsockname(socket, &tmp_addr, &tmp_addr_len);
  if (raw_error == -1) {
    return -1;
  }
  addr_len = tmp_addr_len;
  return 0;
}

int GetAddress1111(const std::byte *sock_addr, int buffer_len, std::string &address) {
  sockaddr &tmp_addr = (struct sockaddr &)*sock_addr;
  int ret = 0;
  switch (tmp_addr.sa_family) {
    case AF_UNIX: {
      struct sockaddr_un *sockaddr_un = reinterpret_cast<struct sockaddr_un *>(&tmp_addr);
      address = sockaddr_un->sun_path;
      break;
    }
    case AF_INET: {
      char addr_buff[INET_ADDRSTRLEN] ; // Use INET6_ADDRSTRLEN to accommodate IPv6 addresses
      struct sockaddr_in *sockaddr_in = reinterpret_cast<struct sockaddr_in *>(&tmp_addr);
      if (::inet_ntop(AF_INET, &(sockaddr_in->sin_addr), addr_buff, INET_ADDRSTRLEN) != nullptr) {
        address = addr_buff;
      }
      break;
    }
    case AF_INET6: {
      char addr_buff[INET6_ADDRSTRLEN];
      struct sockaddr_in6 *sockaddr_in6 = reinterpret_cast<struct sockaddr_in6 *>(&tmp_addr);
      if (::inet_ntop(AF_INET6, &(sockaddr_in6->sin6_addr), addr_buff, INET6_ADDRSTRLEN) != nullptr) {
        address = addr_buff;
      }
      break;
    }
    default:
      std::cerr << "Unsupported address family: " << tmp_addr.sa_family << std::endl;
      ret = -1;
      break;
  }
  return ret;
}

void test() {
//  int fd = BindSocket();
//  int fd = BindIpv4Socket();
  int fd = BindUnixSocket();

  if (fd == -1) {
    return; // Early return if socket binding fails
  }

  std::array<std::byte,512> addr_buff;
  int addr_len = 512;
  if (GetSockName1111(fd, addr_buff.data(), addr_len) == -1) {
    perror("getsockname error");
    close(fd); // Ensure the socket is closed on error
    return;
  }

//  struct sockaddr tmp_addr1 {};
//  std::memcpy(&tmp_addr1, addr_buff, addr_len);

  std::string tmp_address;
  if (GetAddress1111(addr_buff.data(), addr_len, tmp_address) == -1) {
    close(fd); // Ensure the socket is closed on error
    return;
  }
  /* Print socket name */
  printf("Socket name: %s\n", tmp_address.c_str());

  close(fd);
}

int main() {
  test();
  return 0;
}