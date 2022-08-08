/**
 * @file epoller.cc
 * @author your name (you@domain.com)
 * @brief EPOLL functions
 * @version 0.1
 * @date 2021-12-06
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef EPOLLER_H_
#define EPOLLER_H_

#include <sys/epoll.h>

#include <map>
#include <memory>
#include <vector>

#include "channel.h"

class Channel;

enum EPollEvent { READ_TYPE, WRITE_TYPE, ERROR_TYPE };

class EPoller {
 public:
  EPoller();
  ~EPoller();

  // void EPollCtrl(int fd, int option, epoll_event* event);
  void EPollCtrl(std::shared_ptr<Channel> channel, int option,
                 epoll_event* event);
  void EPollWait();
  int EventType(epoll_event event);
  int epfd();

  const int kMaxEventNum = 1024;
  const int kEpollTimeout = 1000;

 private:
  int epfd_;
  // std::vector<int> fds_;
  std::map<int, std::shared_ptr<Channel>> channels_;
};

#endif