
#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <vector>
#include <map>
#include <string>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
// #include "Mesh.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "common/Mesh.h"

const unsigned int NUM_BONES_PER_VERTEX = 4;
//class Skeleton;


struct VertexBoneData
{
    /* de pastrat numarul de elemente al IDs & Weights */
    unsigned int IDs[NUM_BONES_PER_VERTEX];
    float Weights[NUM_BONES_PER_VERTEX];
    
    VertexBoneData()
    {
        Reset();
    }
    
    void Reset()
    {
        for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; ++i)
        {
            IDs[i] = 0;
            Weights[i] = 0;
        }
    }
    /* Adds a new Bone */
    void AddBoneData(unsigned int BoneID, float Weight)
    {
        for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; ++i)
            if (Weights[i] == 0.0)
            {
                IDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
    }
};

struct BoneInfo
{
    glm::mat4 BoneOffset;
    glm::mat4 FinalTransformation;
    
    BoneInfo()
    {
        BoneOffset = glm::mat4(0.0f);
        FinalTransformation = glm::mat4(0.0f);
    }
};




class Object3D
{
private:
    bool flac_lib;
    vector<Mesh> meshes;
    map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
    unsigned int m_NumBones;
    std::vector<BoneInfo> bonesInfo;
    // Access specifier
public:
    Object3D();
    Object3D(std::string filename, bool flac_lib, float scale = 1.0);
    ~Object3D();
    
    void setShaders(std::string vfile, std::string ffile);
    void loadMesh();
    void draw();
    void draw_assimp();
    bool myLoadAssImp(const char *path, std::vector<unsigned int> &indices, std::vector<glm::vec3> &vertices,
                      std::vector<glm::vec2> &uvs, std::vector<glm::vec3> &normals, std::vector<VertexBoneData> &bones);
    void loadTmre(const std::string& fileName);
    void InitMesh(unsigned int MeshIndex,
                  const aiMesh* paiMesh,
                  std::vector<glm::vec3>& Positions,
                  std::vector<glm::vec3>& Normals,
                  std::vector<glm::vec2>& TexCoords,
                  std::vector<VertexBoneData>& Bones,
                  std::vector<unsigned int>& Indices);
    void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);
    
    std::string vertShaderFilename;
    std::string fragShaderFilename;
    std::string meshFilename;
    std::string textureFilename;
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<VertexBoneData> bones;
    
    GLuint programID;
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    GLuint TextureID;
    GLuint TextureHandler;
    GLuint VertexArrayID; //VAO
    GLuint LightID;
    
    GLuint Texture;
    GLuint vertexbuffer;
    GLuint normalbuffer;
    GLuint uvbuffer;
    GLuint bonebuffer;
    
    glm::mat4 ModelMatrix;
    
    glm::vec4 lightPos;
    float load_scale;
    
//    Skeleton *skel;
};

#endif
