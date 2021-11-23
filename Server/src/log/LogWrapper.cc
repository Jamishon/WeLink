/**
 * @file LogWrapper.cc
 * @author Jamishon
 * @brief  Log4cplus wrapper class
 * @version 0.1
 * @date 2021-11-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "LogWrapper.h"

#include <log4cplus/configurator.h>

LogWrapper::LogWrapper() { log4cplus::initialize(); }

LogWrapper::~LogWrapper() { log4cplus::Logger::shutdown(); }

bool LogWrapper::Init(const std::string& log_config_path,
                      const std::string& logger_name) {
  int result = true;
  try {

    // multi-thread supported
    // static log4cplus::ConfigureAndWatchThread configure_watch_thread(
    //   LOG4CPLUS_TEXT(log_config_path), watch_interval_);
    
    log4cplus::PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(log_config_path));
    
    if (!log4cplus::Logger::exists(LOG4CPLUS_TEXT(logger_name)))
      result = false;

    logger_ = log4cplus::Logger::getInstance(
        LOG4CPLUS_TEXT(logger_name));

  } catch (...) {
    result = false;
  }

  return result;
}