#include "Log.h"
#include <sys/stat.h>
#include <dirent.h>
#include <thread>
#include <unistd.h>
#include <sstream>
#include <time.h>
#include <cstdarg>
#include <cstdio>
#include <iostream>


bool Log::exit_write = false;
std::list<std::string> Log::list_write_lines;
FILE* Log::pfile_log = nullptr;
std::unique_ptr<std::thread> Log::up_thread;
std::condition_variable Log::cv_write;
std::mutex Log::mx_write;
int Log::log_level = LOG_LEVEL_TRACE;
bool Log::truncate_line = false;
int64_t Log::log_file_size = LOG_FILE_SIZE;
std::string Log::log_file_path = "";
std::string Log::process_id = "";
std::vector<std::string> Log::vt_level = {"[TRACE]", "[DEBUG]", "[INFO]", "[WARNING]", "[ERROR]", "[FATAL]"};
int64_t Log::log_write_size = 0;


void Log::Init(const char* log_folder, bool truncate, int64_t file_size) {
    if(!log_folder) {
        return;
    }

    truncate_line = truncate;
    log_file_size = file_size;

    DIR* dir = opendir(log_folder);
    if( dir == NULL ) {
        if( mkdir(log_folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0 ) {
            return;
        }
    }

    closedir(dir);

    log_file_path = log_folder;

    char pid[32] = { 0 };
    snprintf(pid, sizeof(pid), "%05d", (int)::getpid());
    process_id = pid;
    

    up_thread.reset(new std::thread(WriteProc));

}

void Log::Uninit() {
    exit_write = true;

    cv_write.notify_one();

    if(up_thread->joinable())
      up_thread->join();

    if(pfile_log) {
        fclose(pfile_log);
        pfile_log = nullptr;
    }
      
  
}

void Log::Output(int level, const char* filename, int lineno, const char* pformat, ...) {
  if( level < log_level )
    return;
  
  std::string line;
  LinePrefix(level, line);

  char file_line[512] = { 0 };
  snprintf(file_line, sizeof(file_line), "[%s:%d]", filename, lineno);
  line.append(file_line);
  
  std::string log;

  va_list ap;
  va_start(ap, pformat);
  int msg_length = vsnprintf(NULL, 0, pformat, ap);
  va_end(ap);
  if((int)log.capacity() < msg_length + 1) {
    log.resize(msg_length + 1);
  }

  va_list val;
  va_start(val, pformat);
  vsnprintf((char *)log.data(), log.capacity(), pformat, val);
  va_end(val);

  std::string log_content;
  log_content.append(log.c_str(), msg_length);

  if(truncate_line)
    log_content = log_content.substr(0, MAX_LINE_LENGTH);

  line.append(log_content);
  line.append("\n");
  

  std::lock_guard<std::mutex> guard(mx_write);
  list_write_lines.push_back(line);
  cv_write.notify_one();

}

void Log::WriteProc() {

    while(true) {

      if(pfile_log == nullptr || log_write_size >= log_file_size) {
        time_t tt = time(NULL);
        tm t;
        localtime_r(&tt, &t);

        char time[64];
        strftime(time, sizeof(time), "%Y%m%d%H%M%S", &t);

        std::string filename(log_file_path);
        filename += time;
        filename += ".";
        filename += process_id;
        filename += ".log";

        if(!CreateFile(filename))
          return;
      }

      std::string line;
      {
        std::unique_lock<std::mutex> guard(mx_write);
        while(list_write_lines.empty()) {

          if(exit_write) return;

          cv_write.wait(guard);
        }

        line = list_write_lines.front();
        list_write_lines.pop_front();

      }

      std::cout << line << std::endl;

      if(WriteLine(line)) {
        log_write_size += line.size();
      }

    }

}

void Log::LinePrefix(int level, std::string &line) {
  line = vt_level[level % vt_level.size()];

  char time[64] = { 0 };
  GetNow(time, sizeof(time));

  line.append("[");
  line.append(time);
  line.append("]");

  char thread_id[32] = { 0 };
  std::ostringstream oss;
  oss << std::this_thread::get_id();
  snprintf(thread_id, sizeof(thread_id), "[%s]", oss.str().c_str());

  line.append(thread_id);
    
}

void Log::GetNow(char* time, int length) {
    //struct timeb tb;
    //ftime(&tb);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
  
    time_t now = ts.tv_sec;
    tm timenow;
    localtime_r(&now, &timenow);

    snprintf(time, length, "%04d-%02d-%02d %02d:%02d:%02d:%03ld", 
             timenow.tm_year + 1900, timenow.tm_mon + 1, timenow.tm_mday,
             timenow.tm_hour, timenow.tm_min, timenow.tm_sec, ts.tv_nsec/1000000);      
}       

bool Log::CreateFile(std::string file_path) {
  if(pfile_log) fclose(pfile_log);

  pfile_log = ::fopen(file_path.c_str(), "w+");
  return pfile_log != nullptr;
}

bool Log::WriteLine(std::string line) {
  bool result = false;

  if(pfile_log) {
    int written_len = 0;
  
    while ( true ) {
      written_len = fwrite(line.c_str(), sizeof(char), line.length(), pfile_log);
      if( written_len <= 0) {
        break;
      } else if (written_len <= (int)line.length()) {
        line.erase(0, written_len);
      }

      if(line.empty()) { 
        result = true;
        break;
      }
    }

  }
  return result;  
}