//
//  Mesh.hpp
//  myAnimtion
//
//  Created by David Choqueluque Roman on 3/1/19.
//  Copyright © 2019 David Choqueluque Roman. All rights reserved.
//

#ifndef Mesh_hpp
#define Mesh_hpp

#include <stdio.h>
#include <iostream>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include "sk_class/my_math.h"

#include "common/controls.hpp"

using namespace std;
#define INVALID_MATERIAL 0xFFFFFFFF

const int num_Buffers = 5;
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
struct Mesh {
    Mesh()
    {
        NumIndices = 0;
        BaseVertex = 0;
        BaseIndex = 0;
        MaterialIndex = INVALID_MATERIAL;
    }
    
    unsigned int NumIndices;
    unsigned int BaseVertex;
    unsigned int BaseIndex;
    unsigned int MaterialIndex;
};

enum VB_TYPES {
    INDEX_BUFFER,
    POS_VB,
    NORMAL_VB,
    TEXCOORD_VB,
    BONE_VB,
    NUM_VBs
};


class Mesh_Skinning{
    GLuint VAO;
    GLuint Buffers[NUM_VBs];
    
    vector<Mesh> meshes;
    map<string, unsigned int> m_BoneMapping; // maps a bone name to its index
    unsigned int NumBones;
    vector<BoneInfo> bonesInfo;
    const aiScene* pScene;
    Assimp::Importer Importer;
    
    /* duration of the animation, can be changed if frames are not present in all interval */
    double animDuration;
    Matrix4f GlobalInverseTransform;
    
public:
    GLuint programID;
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    GLuint TextureID;
    GLuint TextureHandler;
    GLuint VertexArrayID; //VAO
    GLuint LightID;
    glm::mat4 ModelMatrix;
    glm::vec4 lightPos;
    GLuint Texture;
    
    Mesh_Skinning();
    Mesh_Skinning(string file);
    ~Mesh_Skinning();
    
    bool load(const std::string& fileName);
    void InitMesh(unsigned int MeshIndex,
                  const aiMesh* paiMesh,
                  std::vector<Vector3f>& Positions,
                  std::vector<Vector3f>& Normals,
                  std::vector<Vector2f>& TexCoords,
                  std::vector<VertexBoneData>& Bones,
                  std::vector<unsigned int>& Indices);
    void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);
    // void CalcFPS();
    void BoneTransform(float TimeInSeconds, std::vector<glm::mat4>& Transforms);
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
    unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void Clear();
    bool load_from_scene();
    void render();
    
};

#endif /* Mesh_hpp */
