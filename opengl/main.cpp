
#include "Config.h"
#include "ApplicationFactory.h"

#include <string>
#include <vector>
#include <iostream>

int main(int argc, char** argv)
{
    Nova::Config config;
    std::vector<std::string> configuration_paths;
    configuration_paths.push_back("/etc/nova.conf");
    configuration_paths.push_back("~/.config/nova.conf");
    configuration_paths.push_back("./nova.conf");
    config.Parse(configuration_paths, argc, argv);

    Nova::ApplicationFactory app( config );
    app.Run();


    return 0;
}
