//#####################################################################
// Copyright (c) 2016, Mridul Aanjaneya.
//#####################################################################
#include "AssimpRenderable_Mesh.h"
#include <sstream>
#include <iostream>
using namespace Nova;
//#####################################################################
// Constructor
//#####################################################################
AssimpRenderable_Mesh::
AssimpRenderable_Mesh(const std::vector<Vertex>& vertices_input,const std::vector<GLuint>& indices_input,const std::vector<Texture>& textures_input)
    :vertices(vertices_input),indices(indices_input),textures(textures_input)
{
    Setup_Mesh();
}
//#####################################################################
// Setup_Mesh
//#####################################################################
void AssimpRenderable_Mesh::
Setup_Mesh()
{
    // create buffers/arrays
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(Vertex),&vertices[0],GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(GLuint),&indices[0],GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,normal));
    // vertex texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(GLvoid*)offsetof(Vertex,tex_coords));
    glBindVertexArray(0);
}
//#####################################################################
// Draw
//#####################################################################
void AssimpRenderable_Mesh::
Draw(Shader& shader)
{
    // bind appropriate textures
    uint32_t texture_flags = 0;
    for(GLuint i=0;i<textures.size();++i)
    {
        glActiveTexture(GL_TEXTURE0+i);                             // active proper texture unit before binding
        // retrieve texture number
        std::stringstream ss;std::string number;
        std::string name=textures[i].string_type();
        // set the sampler to the correct texture unit
        shader.SetInteger( (name).c_str(), i );
        // bind the texture
        glBindTexture(GL_TEXTURE_2D,textures[i].id);
        texture_flags |= textures[i].type;
    }

    // set flags indicating which textures are active
    shader.SetUInteger( "texture_flags", texture_flags );

    // draw
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_INT,0);
    glBindVertexArray(0);

    // set everything to default once configured
    for(GLuint i=0;i<textures.size();++i)
    {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D,0);
    }
}
//#####################################################################
