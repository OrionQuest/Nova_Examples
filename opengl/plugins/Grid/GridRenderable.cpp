//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
#include <nova/Tools/Grids/Grid.h>
#include <nova/Tools/Utilities/File_Utilities.h>
#include "../../RenderableManager.h"
#include "../../ApplicationFactory.h"
#include "../../Shader.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Nova{
class GridRenderable: public Renderable
{
    typedef float T;typedef Vector<T,3> TV;
    typedef Vector<int,3> T_INDEX;
    using BASE  = Renderable;

    std::array< glm::vec3, 8> VoxelVertices;
    std::array< GLuint, 24> VoxelIndices;
    std::vector< glm::vec3 > VoxelPositions;

    unsigned int VAO,VBO,EBO,instanceVBO;

    glm::vec3 _slice_start;
    float _dx;
    glm::vec3 _max_slice;
    glm::vec4 _bounding_sphere;

  public:
    GridRenderable(ApplicationFactory& app)
        :BASE(app)
    {}

    virtual ~GridRenderable() {}

    virtual void load(std::string path)
    {
        Grid<float,3> grid;
        std::cout<<"Loading: "<<path<<std::endl;
        File_Utilities::Read_From_File(path,grid);

        // Load all cell centers as VoxelPositions
        VoxelPositions.reserve( grid.counts[0] * grid.counts[1] * grid.counts[2] );
        for(int i=0;i<grid.counts[0];++i) for(int j=0;j<grid.counts[1];++j) for(int k=0;k<grid.counts[2];++k)
        {
            TV center=grid.Center(T_INDEX({i,j,k}));
            glm::vec3 location={center[0],center[1],center[2]};
                                  
            VoxelPositions.push_back( location );
        }

        // Build shape for single voxel
        VoxelVertices[0] = glm::vec3( -.5, -.5, -.5 ) * grid.dX[0];
        VoxelVertices[1] = glm::vec3( -.5, -.5,  .5 ) * grid.dX[0];
        VoxelVertices[2] = glm::vec3( -.5,  .5, -.5 ) * grid.dX[0];
        VoxelVertices[3] = glm::vec3( -.5,  .5,  .5 ) * grid.dX[0];
        VoxelVertices[4] = glm::vec3(  .5, -.5, -.5 ) * grid.dX[0];
        VoxelVertices[5] = glm::vec3(  .5, -.5,  .5 ) * grid.dX[0];
        VoxelVertices[6] = glm::vec3(  .5,  .5, -.5 ) * grid.dX[0];
        VoxelVertices[7] = glm::vec3(  .5,  .5,  .5 ) * grid.dX[0];
        VoxelIndices[0] = 0; VoxelIndices[1] = 1;
        VoxelIndices[2] = 3; VoxelIndices[3] = 2;
        VoxelIndices[4] = 0; VoxelIndices[5] = 100;
        VoxelIndices[6] = 4; VoxelIndices[7] = 5;
        VoxelIndices[8] = 7; VoxelIndices[9] = 6;
        VoxelIndices[10] = 4; VoxelIndices[11] = 100;
        VoxelIndices[12] = 0; VoxelIndices[13] = 4;
        VoxelIndices[14] = 100;
        VoxelIndices[15] = 1; VoxelIndices[16] = 5;
        VoxelIndices[17] = 100;
        VoxelIndices[18] = 2; VoxelIndices[19] = 6;
        VoxelIndices[20] = 100;
        VoxelIndices[21] = 3; VoxelIndices[22] = 7;
        VoxelIndices[23] = 100;

        

        _slice_start.x = grid.domain.min_corner[0]+ grid.dX[0]/2.0f;
        _slice_start.y = grid.domain.min_corner[1]+ grid.dX[1]/2.0f;
        _slice_start.z = grid.domain.min_corner[2]+ grid.dX[2]/2.0f;
        _dx = grid.dX[0];
        _max_slice.x = grid.counts[0]-1; // We slice on cells, so one less than the node count
        _max_slice.y = grid.counts[1]-1;
        _max_slice.z = grid.counts[2]-1;

        glm::vec3 sphere_center;
        sphere_center.x = grid.domain.min_corner[0];
        sphere_center.y = grid.domain.min_corner[1];
        sphere_center.z = grid.domain.min_corner[2];
        sphere_center = glm::mix( sphere_center, _max_slice * _dx, 0.5f);
        float sphere_radius = glm::distance( sphere_center, glm::vec3( _max_slice * _dx ) );
        _bounding_sphere = glm::vec4( sphere_center.x, sphere_center.y, sphere_center.z, sphere_radius );
    }

    virtual void initializeBuffers()
    {
        glGenVertexArrays(1,&VAO);
        glGenBuffers(1,&VBO);
        glGenBuffers(1, &EBO);
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(VAO);        

        // First setup all buffers and attributes for the single voxel prototype
        // Setup buffer for Voxel Vertex data
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        glBufferData(GL_ARRAY_BUFFER,8*sizeof(glm::vec3),&VoxelVertices[0],GL_STATIC_DRAW);
        // Setup buffer for Voxel Index data
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24*sizeof(GLuint), &VoxelIndices[0], GL_STATIC_DRAW); 
        // Setup Attribute for Voxel Vertex data
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),(GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        // Next, setup all instance specific buffers and attributes
        // Setup buffer for instanced data
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*VoxelPositions.size(), &VoxelPositions[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glVertexAttribDivisor(1, // Attribute 1 is our instance-specific offset 
                              1  // And we want to update it every instance increment
                              );

        glBindVertexArray(0);
    }

    virtual void draw()
    {
        glm::mat4 projection,view,model;
        view = _app.GetWorld().Get_ViewMatrix();
        model = _app.GetWorld().Get_ModelMatrix();
        projection = _app.GetWorld().Get_ProjectionMatrix();
        auto slicePlanes = _app.GetWorld().Get_Slice_Planes();

        auto shader = _app.GetShaderManager().GetShader("InstancedGrid");
        shader->SetMatrix4("projection",projection);
        shader->SetMatrix4("view",view);
        shader->SetMatrix4("model",model);
        shader->SetVector4f("slice_plane0",slicePlanes[0]);
        shader->SetVector4f("slice_plane1",slicePlanes[1]);
        
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(100); // We use this to identify restart indices
        
        glBindVertexArray(VAO);
        glDrawElementsInstanced(GL_LINE_STRIP,24,GL_UNSIGNED_INT, 0, VoxelPositions.size());
        glBindVertexArray(0);

        glDisable(GL_PRIMITIVE_RESTART);
    }

    virtual bool selectable() {return false;}

    virtual float hit_test(glm::vec3 start_point,glm::vec3 end_point)
    {return 0.;}

    virtual glm::vec4 bounding_sphere()
    {return _bounding_sphere;}

    virtual void assign_selection(glm::vec3 start_point,glm::vec3 end_point,glm::vec3 intersection)
    {}

    virtual void unassign_selection()
    {}

    virtual bool slice_provider() {return true;};

    virtual glm::vec3 slice_start() {
        return _slice_start; 
    };

    virtual float slice_dx() {
        return _dx;
    };
    
    virtual glm::vec3 max_slice() {
        return _max_slice;
    };
    
};

class GridRenderableFactory: public RenderableFactory
{
    using BASE  = RenderableFactory;

  public:
    GridRenderableFactory()
        :BASE()
    {}

    virtual ~GridRenderableFactory() {}

    virtual std::unique_ptr<Renderable> Create(ApplicationFactory& app,std::string path)
    {
        GridRenderable* renderable = new GridRenderable(app);
        renderable->load(path);
        renderable->initializeBuffers();
        return std::unique_ptr<Renderable>(renderable);
    }

    virtual bool AcceptExtension(std::string ext) const
    {
        if(ext=="grid") return true;
        else return false;
    }
};
}

extern "C" void registerPlugin(Nova::ApplicationFactory& app)
{
    app.GetRenderableManager().AddFactory(std::move(std::unique_ptr<Nova::RenderableFactory>(new Nova::GridRenderableFactory())));
}

extern "C" int getEngineVersion()
{
    return Nova::API_VERSION;
}
