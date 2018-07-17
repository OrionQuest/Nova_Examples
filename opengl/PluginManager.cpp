#include "PluginManager.h"
#include "ApplicationFactory.h"
#include <exception>
#include <iostream>
#include <utility>

Nova::PluginManager::PluginManager(ApplicationFactory& app) : _app(app)
{
  ClearSearchPaths();
}

void
Nova::PluginManager::LoadPlugin( std::string name ){
    auto res = _plugins.find( name );
    if( res == _plugins.end() ){
        //std::cout << "Requested plugin '"<< name << "' not loaded. Searching for plugin." << std::endl;
        
        std::vector< std::string > plugin_paths = GetAllPaths( name+Plugin::DefaultExtension() );
        for( auto potential_path : plugin_paths ){
            //std::cout << "Trying to load '" << potential_path << "'"<< std::endl;
            
            std::unique_ptr<Plugin> newPlugin;
            // First try to load the plugin
            try{
                newPlugin = std::unique_ptr<Plugin>( new Plugin(potential_path) );
            }
            catch( std::exception& e ){
                std::cout << "Plugin '"<< potential_path <<"' failed to load: " << e.what() << std::endl;
                continue;
            }
            
            // Check to match kernel version
            if( newPlugin->getEngineVersion() != Nova::API_VERSION ){
                std::cout << "Plugin API version mismatch. Plugin cannot be loaded." << std::endl;
                continue;
            }
            
            // Register the plugin and add it to the list
            newPlugin->registerPlugin( _app );
            std::cout << "Plugin " << name << " was loaded successfully." << std::endl;
            _plugins.insert( std::make_pair( name, std::move(newPlugin) ) );
            return;
        }
    }
}
 
