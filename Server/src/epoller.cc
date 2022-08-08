/**
 * @file epoller.cc
 * @author Jamishon
 * @brief Deal with epoll event
 * @version 0.1
 * @date 2021-12-06
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "epoller.h"

#include "log/LogWrapper.h"
#include "unistd.h"

EPoller::EPoller() {
  epfd_ = ::epoll_create(1);
  if (epfd_ < 0) {
    LOG_ERROR("EPoller::EPoller");
  }
}

EPoller::~EPoller() {
  channels_.clear();
  ::close(epfd_);
}

// void EPoller::EPollCtrl(int fd, int option, epoll_event* event) {
//   int ret = ::epoll_ctl(epfd_, option, fd, event);
//   if (ret < 0) {
//     LOG_ERROR("EPoller::EPollCtrl ret:" << ret);
//   }
// }

void EPoller::EPollCtrl(std::shared_ptr<Channel> channel, int option,
                        epoll_event* event) {
  int ret = ::epoll_ctl(epfd_, option, channel->fd(), event);
  if (ret < 0) {
    LOG_ERROR("EPoller::EPollCtrl ret:" << ret);
  } else {
    if (option == EPOLL_CTL_ADD) {
      channels_[channel->fd()] = channel;
    } else if (option == EPOLL_CTL_MOD) {
    } else if (option == EPOLL_CTL_DEL) {
      channels_.erase(channel->fd());
    }
  }

  LOG_INFO("EPoller::EPollCtrl channel epfd:" << epfd_ << " fd:" << channel->fd()
                                            << " option:" << option
                                            << " event:" << event->events);
}

int EPoller::EventType(epoll_event event) {
  if (event.events & (EPOLLIN | EPOLLPRI))
    return READ_TYPE;
  else if (event.events & EPOLLOUT)
    return WRITE_TYPE;
  else
    return ERROR_TYPE;
}

void EPoller::EPollWait() {
  epoll_event events[kMaxEventNum];

  while (true) {
    int ret = ::epoll_wait(epfd_, events, kMaxEventNum, kEpollTimeout);

    if (ret > 0) {
      for (int i=0; i<ret; i++) {
        epoll_event event = events[i];  
        LOG_INFO("EPoller::EPollWait i:" << i << " event fd:" << event.data.fd << " event:" << event.events);  
        std::shared_ptr<Channel> channel = channels_.at(event.data.fd);
        switch (EventType(event)) {
          case READ_TYPE: {
            channel->Receive();
          } break;
          case WRITE_TYPE: {
            channel->Send();
          } break;
          case ERROR_TYPE: {
            int err_no = errno;
            channel->OnError(event.data.fd, err_no);
          } break;
          default:
            break;
        }
      }
    } else if (ret == 0) {
      continue;
    } else if (ret < 0) {
      int err_no = errno;
      if (err_no == EINTR)
        continue;
      else
        break;
    }
  }
}

int EPoller::epfd() {
  return epfd_;
}
