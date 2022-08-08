/**
 * @file connection.h
 * @author Jamishon
 * @brief Manage the connection status of the sockets
 * @version 0.1
 * @date 2021-11-30
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <functional>
#include <memory>

#include "channel.h"

enum  ConnectonState { CONNECTING, CONNECTED, CLOSEED};

class Connection {
 public:
  typedef std::function<void(void *buf, int buf_len)> ReadCallback;
  typedef std::function<void()> SendDataCompleteCallback;
  explicit Connection(int fd);
  ~Connection();

  int Connect();
  void Disconnect();
  void SendData(void *buf, int but_len);
  ReadCallback read_data_;
  SendDataCompleteCallback send_data_complete_;
  void SetReadDataCallback(ReadCallback read_callback);
  void SetSendDataCompleteCallback(SendDataCompleteCallback callback);

  std::shared_ptr<Channel> GetChannel();

 private:
  std::shared_ptr<Channel> channel_;
  int conn_state_;
};

#endif