/**
 * Asynchronize log
 * 
 * Jamishon
 * 2021/9/24
 */ 

#ifndef LOG_H_
#define LOG_H_

#include <thread>
#include <memory>
#include <list>
#include <condition_variable>
#include <vector>

#define MAX_LINE_LENGTH 256
#define LOG_FILE_SIZE 10 * 1024 * 1024

enum LOG_LEVEL {
    LOG_LEVEL_TRACE,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL
}; 

#define LOGT(...) Log::Output(LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOGD(...) Log::Output(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOGI(...) Log::Output(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOGW(...) Log::Output(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOGE(...) Log::Output(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOGF(...) Log::Output(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

class Log {
public:
    static void Init(const char* log_folder, bool truncate = false, int64_t file_size = 10 * 1024 * 1024);

    static void Uninit();

    static void SetLevel(LOG_LEVEL level);

    static void Output(int level, const char* pfilename, int nlineno, const char* pformat, ...);

    static void WriteProc();

    static void LinePrefix(int level, std::string& line);

    static void GetNow(char* time, int length);

    static bool CreateFile(std::string filepath);

    static bool WriteLine(std::string line);


private:
    Log() = delete;
    ~Log() = delete;
    Log(const Log& rhs) = delete;
    Log& operator=(const Log& rhs) = delete;


    static std::unique_ptr<std::thread>          up_thread;
    static std::list<std::string>           list_write_lines;
    static std::condition_variable          cv_write;
    static std::mutex                       mx_write;
    static FILE*                            pfile_log;
    static std::string                      log_file_path;
    static int                              log_level;
    static bool                             exit_write;
    static int64_t                          log_file_size;
    static int64_t                          log_write_size;
    static bool                             truncate_line;
    static std::string                      process_id;
    static std::vector<std::string>              vt_level;
};

#endif // LOG_H_

