/**
 * Read and write config file.
 * 
 * Jamishon  2021/9/26
 * 
 */ 
#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

#include <map>
#include <cstdio>
#include <string>

class ConfigFile {
public:
    ConfigFile(std::string file_path);
    ~ConfigFile();

    std::string GetConfig(std::string key);
    bool ParseLine(std::string &line);
   
private:
    std::map<std::string, std::string>  config_maps;
};

#endif