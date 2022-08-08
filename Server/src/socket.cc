/**
 * @file socket.cc
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-12-03
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "socket.h"

#include <fcntl.h>
#include <unistd.h>

#include "log/LogWrapper.h"

namespace welink {
namespace socket {

int CreateNonblockSocket(int domain, int type, int protocol) {
  int fd = ::socket(domain, type, protocol);
  if (fd < 0) {
    LOG_FATAL("socket::CreateNonblockSocket error");
  }

  return fd;
}

void SetSocketReuseAddrAndPort(int fd, bool reuse) {
  int on = reuse ? 1 : 0;
  int ret_port = ::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof on);
  if (ret_port < 0) {
    LOG_ERROR("socket::SetSocketReuseAddrAndPort port failed fd:" << fd << " "
                                                                  << reuse);
  }
  int ret_addr = ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof on);
  if (ret_addr < 0) {
    LOG_ERROR("socket::SetSocketReuseAddrAndPort addr failed fd:" << fd << " "
                                                                  << reuse);
  }
}

void SetSocketBlock(int fd, bool block) {
  int flag = ::fcntl(fd, F_GETFL, 0);
  block ? (flag |= O_NONBLOCK) : (flag &= ~O_NONBLOCK);
  ::fcntl(fd, F_SETFL, flag);
}

void Bind(int fd, std::string addr, int port) {
  sockaddr_in saddr;
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = inet_addr(addr.c_str());
  saddr.sin_port = htons(port);
  socklen_t len = sizeof saddr;

  int ret = ::bind(fd, (sockaddr*)&saddr, len);
  if (ret < 0) {
    LOG_ERROR("socket::Bind ip:" << addr << " port:" << port << " fd:" << fd);
  }
}

void Listen(int fd) {
  int ret = ::listen(fd, SOMAXCONN);
  if (ret < 0) {
    LOG_ERROR("socket::Listen fd:" << fd);
  }
}

int Read(int fd, void* buf, std::size_t buf_len) {
  return ::read(fd, buf, buf_len);
}

int Write(int fd, void* buf, std::size_t buf_len) {
  return ::write(fd, buf, buf_len);
}

void Close(int fd) {
  int ret = ::close(fd);
  if(ret < 0) 
    LOG_ERROR("::close ret:" << ret  << " fd:" << fd );
}

void SetSocketNonblockCloseExe(int fd) {
  int flag = ::fcntl(fd, F_GETFL, 0);
  flag |= O_NONBLOCK;
  ::fcntl(fd, F_SETFL, flag);

  flag = ::fcntl(fd, F_GETFD, 0);
  flag |= FD_CLOEXEC;
  ::fcntl(fd, F_SETFD, flag);
}

}  // namespace socket
}  // namespace welink