/*
* Created by boil on 2023/11/10.
*/
#ifndef RENDU_LOG_EXAMPLE_H
#define RENDU_LOG_EXAMPLE_H

#include "net.hpp"

using namespace rendu;
using namespace rendu::net;

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>

#ifdef __APPLE__
#include <sys/event.h>
#endif
#include <cstddef>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>


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
  if (::bind(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(sockaddr_in6)) == -1) {
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
  if (::bind(fd, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) == -1) {
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
  unlink(unix_path.c_str());// Ignore errors, as file may not exist

  strncpy(addr_un.sun_path, unix_path.c_str(), sizeof(addr_un.sun_path) - 1);

  /* Bind socket to address */
  if (bind(fd, reinterpret_cast<struct sockaddr *>(&addr_un), sizeof(sockaddr_un)) == -1) {
    perror("bind error");
    close(fd);
    return -1;
  }
  return fd;
}


SocketError net_example() {
  RD_LOGGER_INIT("net_example", LogLevel::LL_DEBUG);
  //    int fd = BindSocket();
  //    int fd = BindIpv4Socket();
  int fd = BindUnixSocket();
  if (fd == -1) {
    return SocketError::Error;// Early return if socket binding fails
  }
  SafeSocketHandle::Ptr handle(new SafeSocketHandle(fd, true));

  byte sock_address[SocketAddressPal::MaxAddressSize];
  std::span<byte> sock_addr(sock_address, SocketAddressPal::MaxAddressSize);

  SocketError ret = SocketPal::GetSockName(handle, sock_address, SocketAddressPal::MaxAddressSize);
  if (ret == SocketError::Error) {
    perror("getsockname error");
    close(fd);// Ensure the socket is closed on error
    return ret;
  }

  char ipv4_address[SocketAddressPal::MaxAddressSize];
  SocketAddressPal::GetUdsAddress(sock_addr, ipv4_address, SocketAddressPal::MaxAddressSize);
  /* Print socket name */
  printf("Socket name: %s\n", ipv4_address);

  close(fd);

  return SocketError::Success;
}


#endif//RENDU_LOG_EXAMPLE_H
