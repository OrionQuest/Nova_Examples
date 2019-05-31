//!#####################################################################
//! \file main.cpp
//!#####################################################################
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>

#include "Variational_Fluids_Renderable.h"

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace Nova{
class Simulation_Factory: public RenderableFactory
{
    using BASE  = RenderableFactory;

  public:
    Simulation_Factory()
        :BASE()
    {}

    virtual ~Simulation_Factory() {}

    virtual std::string Name()
    {return "Nova:Animation";}

    virtual std::unique_ptr<Renderable> Create(ApplicationFactory& app,std::string path)
    {return std::move(Create_Multiple(app,path).at(0));}

    virtual std::vector<std::unique_ptr<Renderable> > Create_Multiple(ApplicationFactory& app,std::string path)
    {
        fs::path initial_config;
        initial_config /= path;
        std::string directory_name=initial_config.parent_path().string();

        std::vector<std::unique_ptr<Renderable> > renderables;
        std::ifstream conf_file(path.c_str());
        int number_of_frames;
        conf_file >> number_of_frames;
        conf_file.close();

        renderables.push_back(std::move(std::unique_ptr<Renderable>(new Nova::Variational_Fluids_Renderable<float,3>(app,directory_name,number_of_frames))));
        dynamic_cast<Simulation_Renderable<float,3>*>(renderables.back().get())->Load_Active_Frame();

        return renderables;
    }

    virtual int NumRenderables(std::string path)
    {
        return 1;       // TODO: Fix this.
    }

    virtual bool AcceptExtension(std::string ext) const
    {
        if(ext=="nova-animation") return true;
        else return false;
    }
};
}

extern "C" void registerPlugin(Nova::ApplicationFactory& app)
{
    app.GetRenderableManager().AddFactory(std::move(std::unique_ptr<Nova::RenderableFactory>(new Nova::Simulation_Factory())));
}

extern "C" int getEngineVersion()
{
    return Nova::API_VERSION;
}
