/**
 * @file channel_connect.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef CHANNEL_CONNECT_H_
#define CHANNEL_CONNECT_H_

#include "channel.h"

class ChannelConnect : public Channel {
public:
  void Send(void* buf, std::size_t buf_len) override;
};

#endif