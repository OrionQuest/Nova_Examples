//!#####################################################################
//! \file Embedded_Deformables_Renderable.h
//!#####################################################################
// Class Embedded_Deformables_Renderable
//######################################################################
#ifndef __Embedded_Deformables_Renderable_h__
#define __Embedded_Deformables_Renderable_h__

#include <nova/Geometry/Topology_Based_Geometry/Tetrahedralized_Volume.h>
#include <nova/Tools/Utilities/File_Utilities.h>

#include "ViewportManager.h"
#include "Simulation_Renderable.h"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace Nova{
template<class T,int d>
class Embedded_Deformables_Renderable: public Simulation_Renderable<T,d>
{
    using TV                                = Vector<T,d>;
    using BOUNDARY_ELEMENT_INDEX            = Vector<int,d>;
    using INTERIOR_ELEMENT_INDEX            = Vector<int,d+1>;
    using BASE                              = Simulation_Renderable<T,d>;
    using T_Volume                          = Tetrahedralized_Volume<T,d>;
    using T_Surface                         = typename T_Volume::T_Surface;

    using BASE::directory_name;using BASE::active_frame;using BASE::_app;

    T_Volume mesh;
    T_Surface surface;
    unsigned int VAIO,VBIO,EBIO;
    unsigned int VABO,VBBO,EBBO;
    bool selected,draw_volume,draw_surface;

  public:
    Embedded_Deformables_Renderable(ApplicationFactory& app,const std::string& directory_name,int max_frame)
        :BASE(app,directory_name,max_frame),selected(false),draw_volume(true),draw_surface(true)
    {
        app.GetIOService().On("DISPLAY-VOLUME",[&](IOEvent& event){this->Display_Volume();});
        app.GetIOService().On("DISPLAY-SURFACE",[&](IOEvent& event){this->Display_Surface();});
    }

    virtual ~Embedded_Deformables_Renderable() {}

    void Display_Surface()
    {draw_surface=!draw_surface;}

    void Display_Volume()
    {draw_volume=!draw_volume;}

    void Clear_Buffers()
    {
        if(VAIO) glDeleteVertexArrays(1,&VAIO);
        if(VBIO) glDeleteBuffers(1,&VBIO);
        if(EBIO) glDeleteBuffers(1,&EBIO);

        if(VABO) glDeleteVertexArrays(1,&VABO);
        if(VBBO) glDeleteBuffers(1,&VBBO);
        if(EBBO) glDeleteBuffers(1,&EBBO);
    }

    virtual void Initialize_Buffers()
    {
        glGenVertexArrays(1,&VAIO);
        glGenBuffers(1,&VBIO);
        glGenBuffers(1,&EBIO);

        glBindVertexArray(VAIO);
        glBindBuffer(GL_ARRAY_BUFFER,VBIO);
        glBufferData(GL_ARRAY_BUFFER,mesh.points.size()*sizeof(TV),&mesh.points[0],GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,d,GL_FLOAT,GL_FALSE,sizeof(TV),(GLvoid*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBIO);
        if(d==2) glBufferData(GL_ELEMENT_ARRAY_BUFFER,mesh.elements.size()*sizeof(INTERIOR_ELEMENT_INDEX),&mesh.elements[0],GL_STATIC_DRAW);
        else glBufferData(GL_ELEMENT_ARRAY_BUFFER,mesh.boundary_mesh->elements.size()*sizeof(BOUNDARY_ELEMENT_INDEX),&(mesh.boundary_mesh->elements)[0],GL_STATIC_DRAW);
        glBindVertexArray(0);

        glGenVertexArrays(1,&VABO);
        glGenBuffers(1,&VBBO);
        glGenBuffers(1,&EBBO);

        glBindVertexArray(VABO);
        glBindBuffer(GL_ARRAY_BUFFER,VBBO);
        glBufferData(GL_ARRAY_BUFFER,surface.points.size()*sizeof(TV),&surface.points[0],GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,d,GL_FLOAT,GL_FALSE,sizeof(TV),(GLvoid*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,surface.elements.size()*sizeof(BOUNDARY_ELEMENT_INDEX),&surface.elements[0],GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    virtual void Load_Active_Frame() override
    {
        Clear_Buffers();

        if(d==2){File_Utilities::Read_From_File(directory_name+"/"+std::to_string(active_frame)+"/mesh.tri2d",mesh);
            File_Utilities::Read_From_File(directory_name+"/"+std::to_string(active_frame)+"/curve.curve2d",surface);}
        else{File_Utilities::Read_From_File(directory_name+"/"+std::to_string(active_frame)+"/mesh.tet",mesh);
            mesh.Initialize_Boundary_Mesh();
            File_Utilities::Read_From_File(directory_name+"/"+std::to_string(active_frame)+"/surface.tri",surface);}

        Initialize_Buffers();
    }

    void Draw(Shader& shader)
    {
        glBindVertexArray(VAIO);
        if(d==2) glDrawElements(GL_TRIANGLES,mesh.elements.size()*3,GL_UNSIGNED_INT,0);
        else glDrawElements(GL_TRIANGLES,mesh.boundary_mesh->elements.size()*3,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);
    }

    void Draw_Surface(Shader& shader)
    {
        glBindVertexArray(VABO);
        glDrawElements(GL_TRIANGLES,surface.elements.size()*3,GL_UNSIGNED_INT,0);
        glBindVertexArray(0);
    }

    virtual void draw() override
    {
        BASE::draw();

        glm::mat4 projection,view,model;
        view = _app.GetWorld().Get_ViewMatrix();
        model = _app.GetWorld().Get_ModelMatrix();
        projection = _app.GetWorld().Get_ProjectionMatrix();
        auto slicePlanes = _app.GetWorld().Get_Slice_Planes();
        glm::vec3 cameraPos = _app.GetWorld().GetCameraPosition();
        std::vector<glm::vec3> lights = _app.GetWorld().GetSceneLightPositions();

        if(d==2 && draw_surface){
            auto shader = _app.GetShaderManager().GetShader("BasicColored");
            shader->SetMatrix4("projection",projection);
            shader->SetMatrix4("view",view);
            shader->SetMatrix4("model",model);
            shader->SetVector4f("slice_plane0",slicePlanes[0]);
            shader->SetVector4f("slice_plane1",slicePlanes[1]);
            shader->SetVector3f("basecolor",glm::vec3(228/255.0f,26/255.0f,28/255.0f));
            shader->SetInteger("enable_slice",1);

            glBindVertexArray(VABO);
            glDrawElements(GL_LINES,surface.elements.size()*d,GL_UNSIGNED_INT,0);
            glBindVertexArray(0);}

        if(_app.GetWorld().SolidMode() && draw_volume){
            auto _shader = _app.GetShaderManager().GetShader("BasicMeshShader");
            _shader->SetMatrix4("projection",projection);
            _shader->SetMatrix4("view",view);
            _shader->SetMatrix4("model",model);
            _shader->SetVector4f("slice_plane0",slicePlanes[0]);
            _shader->SetVector4f("slice_plane1",slicePlanes[1]);
            _shader->SetInteger("selected",selected?1:0);
            _shader->SetInteger("shaded",_app.GetWorld().LightingMode()?1:2);
            _shader->SetVector3f("cameraPos",cameraPos);
            _shader->SetVector3f("defaultChannels.diffuse",glm::vec3(55/255.0f,126/255.0f,184/255.0f));
            _shader->SetFloat("defaultChannels.shininess",10.0f);
            _shader->SetInteger("activeLights",std::min((int)lights.size(),4));             // no more than 4 lights.
            for(auto l:lights) _shader->SetVector3f("lights[0].position",l);
            Draw(*(_shader.get()));}

        if(_app.GetWorld().WireframeMode() && draw_volume){
            auto _shader = _app.GetShaderManager().GetShader("BasicColored");
            _shader->SetMatrix4("projection",projection);
            _shader->SetMatrix4("view",view);
            _shader->SetMatrix4("model",model);
            _shader->SetVector4f("slice_plane0",slicePlanes[0]);
            _shader->SetVector4f("slice_plane1",slicePlanes[1]);
            _shader->SetVector3f("basecolor",glm::vec3(0.8,0.8,0.9));
            _shader->SetInteger("enable_slice",1);
            
            glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
            glLineWidth(1);
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1,-1);
            Draw(*(_shader.get()));
            glDisable(GL_POLYGON_OFFSET_LINE);
            glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);}

        if(d==3 && draw_surface){
            if(_app.GetWorld().SolidMode()){
                auto _shader = _app.GetShaderManager().GetShader("BasicMeshShader");
                _shader->SetMatrix4("projection",projection);
                _shader->SetMatrix4("view",view);
                _shader->SetMatrix4("model",model);
                _shader->SetVector4f("slice_plane0",slicePlanes[0]);
                _shader->SetVector4f("slice_plane1",slicePlanes[1]);
                _shader->SetInteger("selected",selected?1:0);
                _shader->SetInteger("shaded",_app.GetWorld().LightingMode()?1:2);
                _shader->SetVector3f("cameraPos",cameraPos);
                _shader->SetVector3f("defaultChannels.diffuse",glm::vec3(77/255.0f,175/255.0f,74/255.0f));
                _shader->SetFloat("defaultChannels.shininess",10.0f);
                _shader->SetInteger("activeLights",std::min((int)lights.size(),4));         // no more than 4 lights.
                for(auto l:lights) _shader->SetVector3f("lights[0].position",l);
                Draw_Surface(*(_shader.get()));}

            if(_app.GetWorld().WireframeMode()){
                auto _shader = _app.GetShaderManager().GetShader("BasicColored");
                _shader->SetMatrix4("projection",projection);
                _shader->SetMatrix4("view",view);
                _shader->SetMatrix4("model",model);
                _shader->SetVector4f("slice_plane0",slicePlanes[0]);
                _shader->SetVector4f("slice_plane1",slicePlanes[1]);
                _shader->SetVector3f("basecolor",glm::vec3(0.8,0.9,0.8));
                _shader->SetInteger("enable_slice",1);
                
                glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
                glLineWidth(1);
                glEnable(GL_POLYGON_OFFSET_LINE);
                glPolygonOffset(-1,-1);
                Draw_Surface(*(_shader.get()));
                glDisable(GL_POLYGON_OFFSET_LINE);
                glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);}}
    }
};
}
#endif
