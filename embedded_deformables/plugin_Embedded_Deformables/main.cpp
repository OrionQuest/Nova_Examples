//!#####################################################################
//! \file main.cpp
//!#####################################################################
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>

#include "Embedded_Deformables_Renderable.h"

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

        if(File_Utilities::File_Exists(directory_name+"/common/",false))
        {
            fs::path load_path(directory_name+"/common/");
            fs::directory_iterator it(load_path),eod;
            BOOST_FOREACH(fs::path const &p,std::make_pair(it,eod)) if(fs::is_regular_file(p)){std::string filename(p.c_str());
                if(filename.find_last_of('.')!=std::string::npos){
                    std::string extension=filename.substr(filename.find_last_of('.')+1,filename.size());
                    if(extension=="deformables3d"){
                        renderables.push_back(std::move(std::unique_ptr<Renderable>(new Nova::Embedded_Deformables_Renderable<float,3>(app,directory_name,number_of_frames))));
                        dynamic_cast<Simulation_Renderable<float,3>*>(renderables.back().get())->Load_Active_Frame();}
                    else if(extension=="deformables2d"){
                        renderables.push_back(std::move(std::unique_ptr<Renderable>(new Nova::Embedded_Deformables_Renderable<float,2>(app,directory_name,number_of_frames))));
                        dynamic_cast<Simulation_Renderable<float,2>*>(renderables.back().get())->Load_Active_Frame();}}}
        }

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
