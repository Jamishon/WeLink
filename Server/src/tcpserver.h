/**
 * @file tcpserver.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <map>
#include <string>
#include <memory>
#include <functional>

class EPoller;
class Connection;
class ChannelListen;

class TcpServer {
public:
  typedef std::function<void(const std::shared_ptr<Connection> &)> ConnectionCallback;
  TcpServer(std::string ip, int port);
  ~TcpServer();

  void Start();
  void SetConnectionCallback(const ConnectionCallback &callback);
private:
    //std::map<std::string, std::shared_ptr<Connection>> connections_;
    std::shared_ptr<ChannelListen> channel_listen_;
    std::shared_ptr<EPoller> epoller_;
    ConnectionCallback con_callback_;
};

#endif