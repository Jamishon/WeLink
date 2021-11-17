#include "ConfigFile.h"
#include <fstream>

ConfigFile::ConfigFile(std::string file_path) {
    
    std::ifstream file;
    file.open(file_path);
    std::string line;
    
    while(std::getline(file, line)) {
      ParseLine(line);
    }

    file.close();
}

ConfigFile::~ConfigFile() {
    config_maps.clear();
}

bool ConfigFile::ParseLine(std::string &line) {
    size_t equal = line.find('=');
    if( equal == std::string::npos ) {
        return false;
    }
    size_t comment = line.find("#");
    if( comment != std::string::npos && comment < equal) {
        return false;
    }

    std::string key = line.substr(0, equal);
    std::string value = line.substr(equal + 1);


    std::string whilespaces(" \t\f\v\n\r");
    size_t sf = key.find_first_not_of(whilespaces);
    size_t sl = key.find_last_not_of(whilespaces);
    if(sf <= sl && sf != std::string::npos) {
        key = key.substr(sf, sl - sf + 1);
    } else {
        return false;
    }

    sf = value.find_first_not_of(whilespaces);
    sl = value.find_last_not_of(whilespaces);
    if(sf <= sl && sf != std::string::npos ) {
        value = value.substr(sf, sl - sf + 1);
    } else {
        return false;
    }

    config_maps.insert(std::make_pair(key, value));

    return true;
}

std::string ConfigFile::GetConfig(std::string key) {
    if(key.empty()) return "";

    auto result = config_maps.find(key);
    if( result != config_maps.end()) {
        return result->second;
    }

    return "";
}

