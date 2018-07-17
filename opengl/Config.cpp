#include "Config.h"

#include <boost/assign/list_of.hpp>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <iostream>
#include <fstream>
#include <iterator>

Nova::Config::Config(){    
}

void 
Nova::Config::Parse(const std::vector<std::string>& paths, int argc, char** argv){
    
    std::string config_file;
    // Declare a group of options that will be 
    // allowed only on command line
    po::options_description generic("Generic options");
    generic.add_options()
        ("version,v", "print version string")
        ("help", "produce help message")
        ("config,c", po::value<std::string>(&config_file), "name of a file of a configuration.")
        ("scenepath,s", po::value<std::string>(&scenepath), "path to the scene to display.")        
        ("fontname",po::value<std::string>(&fontname)->default_value(std::string("fonts/arial.ttf")),"path to the font to use for rendering")
        ;
    
    // Declare a group of options that will be 
    // allowed both on command line and in
    // config file
    po::options_description config("Configuration");
    config.add_options()
        ("FPS,f", po::value<float>(&fps)->default_value(24.0f),"frame rate for playback.")
        ("Bind", po::value<std::vector<std::string> >(&bindings)->default_value(std::vector<std::string>(), ""), "Bindings to add to the Keybinding map." )
        ("Plugin", po::value<std::vector<std::string> >(&pluginList), "a list of plugins to load")
        ("PluginSearchPath", po::value<std::vector<std::string> >(&pluginpaths)->default_value(std::vector<std::string>(1,"./plugins"),"./plugins"), "path to search for plugins, in addition to the scene directory.")
        ("ShaderSearchPath", po::value<std::vector<std::string> >(&shaderpaths)->default_value(std::vector<std::string>(1,"./shaders"),"./shaders"), "path to search for shaders, in addition to the scene directory.")
        ("TextureSearchPath", po::value<std::vector<std::string> >(&texturepaths)->default_value(std::vector<std::string>(1,"./textures"),"./textures"), "path to search for textures, in addition to the scene directory.")
        ;
        
    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
        ;
    
    
    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);
    
    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);
    
    po::options_description visible("Allowed options");
    visible.add(generic).add(config);
    
    po::positional_options_description p;
    p.add("scenepath", -1);
    
    try{
        po::variables_map vm;
        store(po::command_line_parser(argc, argv).
              options(cmdline_options).positional(p).run(), vm);
        notify(vm);     
        
        if (vm.count("help")) {
            std::cout << visible << std::endl;;
            exit(0);
        }
    
        if (vm.count("version")) {
            std::cout << "Nova Program, No Version" << std::endl;;
            exit(0);
        }
        
        if( vm.count("config") ){
            std::ifstream ifs(config_file.c_str());
            if (!ifs)
            {
                std::cout << "can not open config file: " << config_file << std::endl;
                return;
            }
            else
                {
                store(parse_config_file(ifs, config_file_options), vm);
                notify(vm);
                }
        }
        else{
            for( auto iter = paths.rbegin(); iter != paths.rend(); iter++ ){
                std::ifstream ifs(iter->c_str());
                if (!ifs)
                    continue;
                else{
                    store(parse_config_file(ifs, config_file_options), vm);
                    notify(vm);
                    break;
                }
            }
        }
    }
    catch( std::exception& e ){
        std::cout << e.what() << std::endl;
        exit(1);
    }
}
