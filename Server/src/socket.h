/**
 * @file socket.cc
 * @author Jamishon
 * @brief  Basic socket function
 * @version 0.1
 * @date 2021-11-25
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef SOCKET_H_
#define SOCKET_H_

#include <arpa/inet.h>
#include <sys/socket.h>

#include <string>

namespace welink {

namespace socket {

int CreateNonblockSocket(int domain = AF_INET,
                         int type = SOCK_CLOEXEC | SOCK_NONBLOCK | SOCK_STREAM,
                         int protocol = IPPROTO_TCP);
void SetSocketReuseAddrAndPort(int fd, bool reuse = true);
void SetSocketBlock(int fd, bool block = true);
void Bind(int fd, std::string ip = INADDR_ANY, int port = 20000);
void Listen(int fd);
void Accept(int fd);
int Read(int fd, void *buf, std::size_t buf_len);
int Write(int fd, void *buf, std::size_t buf_len);
void Close(int fd);

void SetSocketNonblockCloseExe(int fd);

}  // namespace socket

}  // namespace welink

#endif