//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
// Class AssimpRenderable_Model
//##################################################################### 
#ifndef __ASSIMP_RENDERABLE_MODEL_H__
#define __ASSIMP_RENDERABLE_MODEL_H__
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "AssimpRenderable_Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Nova{
    class ApplicationFactory;

class AssimpRenderable_Model
{
    // model data
    std::vector<AssimpRenderable_Mesh> meshes;
    std::string directory;
    std::vector<Texture> textures_loaded;           // stores all the textures loaded so far
    glm::vec4 _boundingSphere;

  public:
    AssimpRenderable_Model(ApplicationFactory& app);
    ~AssimpRenderable_Model() {}

//##################################################################### 
    void Draw( Shader& shader);
    void Load_Model(const std::string& path);
    void Process_Node(aiNode *node,const aiScene *scene);
    AssimpRenderable_Mesh Process_Mesh(aiMesh *mesh,const aiScene *scene);
    void Load_Material_Textures(aiMaterial *material,aiTextureType type,const uint32_t type_name,std::vector<Texture>& textures);
    float TestIntersection( glm::vec3 start_point, glm::vec3 end_point );
    glm::vec4 BoundingSphere();

 private:
    ApplicationFactory& _app;
    void Build_Bounding_Sphere();
//##################################################################### 
};
}
#endif
