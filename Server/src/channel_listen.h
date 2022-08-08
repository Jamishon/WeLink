/**
 * @file channel_listen.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef CHANNEL_LISTEN_H_
#define CHANNEL_LISTEN_H_

#include "channel.h"
#include <memory>
#include "epoller.h"
#include "connection.h"
#include "tcpserver.h"

class ChannelListen : public Channel {
public:
  ChannelListen(int fd, std::string ip, int port);
  ~ChannelListen();

  int Receive() override;
  void SetConnectionCallback(const TcpServer::ConnectionCallback &callback);

private:
  
  std::map<int, std::shared_ptr<Connection>> connection_map_;

  static int assign_connetion_id_;
  std::string ip_;
  int port_;

  TcpServer::ConnectionCallback conn_callback_;

};

#endif