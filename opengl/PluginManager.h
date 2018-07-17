#ifndef __OPENGL_3D_PLUGIN_MANAGER_H__
#define __OPENGL_3D_PLUGIN_MANAGER_H__

#include "Plugin.h"
#include "ResourceManager.h"
#include <map>
#include <vector>
#include <string>
#include <memory>

namespace Nova {

    class ApplicationFactory;

    class PluginManager : public AbstractResourceManager {
    public:
        PluginManager(ApplicationFactory& app);
        virtual ~PluginManager() {};

        void LoadPlugin(std::string name);

    private:
        ApplicationFactory& _app;
        std::map< std::string, std::unique_ptr<Plugin> > _plugins;        
    };

}

#endif
