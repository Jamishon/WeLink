
#include <iostream>
#include "Log.h"
#include <unistd.h>
#include <iostream>

#include "Singleton.h"
#include "ConfigFile.h"


int main(int argc, char* argv[]) {
    Log::Init("logs/");

    LOGT("Hello %s", "world !");

    ConfigFile file("etc/chatserver.conf");
    std::string value = file.GetConfig("dbserver");
    std::cout << value << std::endl;

    Log::Uninit();

}
