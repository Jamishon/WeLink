/**
 * @file channel_listen.cc
 * @author Jamishon
 * @brief  Server listen socket channel
 * @version 0.1
 * @date 2021-12-09
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "channel_listen.h"

#include "log/LogWrapper.h"
#include "socket.h"

int ChannelListen::assign_connetion_id_ = 1;

ChannelListen::ChannelListen(int fd, std::string ip, int port)
    : Channel(fd), ip_(ip), port_(port) {
  welink::socket::Bind(fd, ip, port);
  welink::socket::Listen(fd);
}

ChannelListen::~ChannelListen() {}


int ChannelListen::Receive() {
  sockaddr_in client_addr;
  socklen_t addr_len = sizeof client_addr;
  int fd = ::accept(fd_, (sockaddr *)&client_addr, &addr_len);
  if (fd < 0) {
    int err_no = errno;
    LOG_ERROR("ChannelListen::Receive accept fd:" << fd << " errno" << err_no);
  } else {
    welink::socket::SetSocketNonblockCloseExe(fd);
    std::shared_ptr<Connection> connection = std::make_shared<Connection>(fd);
    connection_map_[assign_connetion_id_++] = connection;

    auto channel = connection->GetChannel();
    channel->SetEPoller(epoller_);
    channel->set_sock_addr(client_addr);

    if (conn_callback_ != nullptr)
      conn_callback_(connection);

    LOG_INFO("ChannelListen::Receive accept() client fd:"
             << fd << " ip:" << channel->GetIp()
             << " port:" << channel->GetPort());
  }

  return 0;
}


void ChannelListen::SetConnectionCallback(const TcpServer::ConnectionCallback &callback) {
  conn_callback_ = callback;
}