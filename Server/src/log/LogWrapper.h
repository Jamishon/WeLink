/**
 * @file LogWrapper.h
 * @author Jamishon
 * @brief  Log4cplus wraaper
 * @version 0.1
 * @date 2021-11-17
 *
 * @copyright Copyright (c) 2021
 *
 */
#ifndef LOGWRAPPER_H_
#define LOGWRAPPER_H_

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include "../Singleton.h"


class LogWrapper final {
 public:
  LogWrapper();
  ~LogWrapper();

  LogWrapper(const LogWrapper&) = delete;
  LogWrapper& operator=(const LogWrapper&) = delete;

  bool Init(const std::string& log_config_path, const std::string& logger_name);

  log4cplus::Logger& GetLogger() { return logger_; }

 private:
  log4cplus::Logger logger_;
  static const unsigned int watch_interval_ = 1 * 60 * 1000;
};

#ifndef LOG_TRACE
#define LOG_TRACE(trace_ostream) \
  LOG4CPLUS_TRACE(Singleton<LogWrapper>::Instance().GetLogger(), trace_ostream)
#endif

#ifndef LOG_DEBUG
#define LOG_DEBUG(debug_ostream) \
  LOG4CPLUS_DEBUG(Singleton<LogWrapper>::Instance().GetLogger(), debug_ostream)
#endif

#ifndef LOG_INFO
#define LOG_INFO(info_ostream) \
  LOG4CPLUS_INFO(Singleton<LogWrapper>::Instance().GetLogger(), info_ostream)
#endif

#ifndef LOG_WARN
#define LOG_WARN(warn_ostream) \
  LOG4CPLUS_WARN(Singleton<LogWrapper>::Instance().GetLogger(), warn_ostream)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(error_ostream) \
  LOG4CPLUS_ERROR(Singleton<LogWrapper>::Instance().GetLogger(), error_ostream)
#endif

#ifndef LOG_FATAL
#define LOG_FATAL(fatal_ostream) \
  LOG4CPLUS_FATAL(Singleton<LogWrapper>::Instance().GetLogger(), fatal_ostream)
#endif

#endif