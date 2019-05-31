//!#####################################################################
//! \file Variational_Fluids_Renderable.h
//!#####################################################################
// Class Variational_Fluids_Renderable
//######################################################################
#ifndef __Variational_Fluids_Renderable_h__
#define __Variational_Fluids_Renderable_h__

#include <nova/Tools/Utilities/File_Utilities.h>
#include <nova/Tools/Vectors/Vector.h>

#include "ViewportManager.h"
#include "plugins/Simulation/Simulation_Renderable.h"

#include <boost/filesystem.hpp>
#include <fstream>
namespace fs = boost::filesystem;

namespace Nova{
template<class T,int d>
class Variational_Fluids_Renderable: public Simulation_Renderable<T,d>
{
    using TV                                = Vector<T,d>;
    using Base                              = Simulation_Renderable<T,d>;

    using Base::directory_name;using Base::active_frame;using Base::_app;

    std::vector<glm::vec3> locations;
    unsigned int VAO,VBO;
    std::string frame_title;
    bool selected;

  public:
    Variational_Fluids_Renderable(ApplicationFactory& app,const std::string& directory_name,int max_frame)
        :Base(app,directory_name,max_frame),selected(false)
    {}

    virtual ~Variational_Fluids_Renderable() {}

    void Clear_Buffers()
    {
        if(VAO) glDeleteVertexArrays(1,&VAO);
        if(VBO) glDeleteBuffers(1,&VBO);

        locations.clear();
    }

    virtual void Initialize_Buffers()
    {
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,locations.size()*sizeof(glm::vec3),&locations[0],GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),(GLvoid*)0);
        glBindVertexArray(0);
    }

    virtual void Load_Active_Frame() override
    {
        Clear_Buffers();

        std::string filename =  directory_name+"/particles_"+std::to_string(active_frame)+".txt";
        std::fstream inFile(filename,std::ios::in);
        int number_of_particles;T radius;
        inFile>>number_of_particles>>radius;

        for(int i=0;i<number_of_particles;++i){glm::vec3 X;
            for(int v=0;v<d;++v) inFile>>X[v];
            locations.push_back(X);}

        Initialize_Buffers();
    }

    virtual void draw() override
    {
        Base::draw();

        glm::mat4 projection,view,model;
        view = _app.GetWorld().Get_ViewMatrix();
        model = _app.GetWorld().Get_ModelMatrix();
        projection = _app.GetWorld().Get_ProjectionMatrix();
        auto slicePlanes = _app.GetWorld().Get_Slice_Planes();

        auto shader = _app.GetShaderManager().GetShader("Points");
        shader->SetMatrix4("projection",projection);
        shader->SetMatrix4("view",view);
        shader->SetMatrix4("model",model);
        shader->SetVector4f("slice_plane0",slicePlanes[0]);
        shader->SetVector4f("slice_plane1",slicePlanes[1]);
        glBindVertexArray(VAO);
        glEnable(GL_PROGRAM_POINT_SIZE);
        shader->SetVector3f("point_color",glm::vec3(.9,.9,.9));
        glDrawArrays(GL_POINTS,0,locations.size());
        glBindVertexArray(0);

        if( _app.GetWorld().GetViewportManager().IsPrimaryViewport() )
            _app.GetTextRenderingService() << frame_title;
    }
};
}
#endif
