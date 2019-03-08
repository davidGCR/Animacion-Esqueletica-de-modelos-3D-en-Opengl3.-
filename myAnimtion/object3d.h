
#ifndef OBJECT3D_H
#define OBJECT3D_H

#include <vector>
#include <map>
#include <string>
//#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
// #include "Mesh.h"
#include<map>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "common/Mesh.h"
#include "sk_class/my_math.h"
#include <deque>
#include "sk_class/Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <chrono>



using namespace std;

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

//struct BoneInfo
//{
//    glm::mat4 BoneOffset;
//    glm::mat4 FinalTransformation;
//
//    BoneInfo()
//    {
//        BoneOffset = glm::mat4(0.0f);
//        FinalTransformation = glm::mat4(0.0f);
//    }
//};
struct BoneInfo
{
    Matrix4f BoneOffset;
    Matrix4f FinalTransformation;
    
//    Matrix4f restTransformation;
////    Transform TRANSF_REST;
////    Transform TRANSF_POSE;
//    Matrix4f Rotation;
    Quaternion quaternion;
//    Matrix4f traslation;
//    Matrix4f scale;
//    Matrix4f ParentTransform;
    
    BoneInfo()
    {
        BoneOffset.SetZero();
        FinalTransformation.SetZero();
//        restTransformation.SetZero();
//        quaternion = Quaternion(0,0,0,0);
    }
};



class Object3D
{
private:
    bool flac_lib;
    vector<Mesh> meshes;
    map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
    unsigned int m_NumBones;
    
    // Access specifier
public:
    //members
    std::vector<BoneInfo> bonesInfo;
    const aiScene* pScene;
    Assimp::Importer m_Importer;
    map<int,Vector3f> CoRs;
    
    /* duration of the animation, can be changed if frames are not present in all interval */
    double animDuration;
    Matrix4f m_GlobalInverseTransform;
    
    std::string vertShaderFilename;
    std::string fragShaderFilename;
    std::string meshFilename;
    std::string textureFilename;
    
    std::vector<Vector3f> vertices;
    std::vector<Vector2f> uvs;
    std::vector<Vector3f> normals;
    std::vector<unsigned int> indices;
    std::vector<VertexBoneData> bones;
    std::vector<Vector3f> cors;
    
    deque<Matrix4f> restTransformDeque;
    
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
    GLuint indexbuffer;
    GLuint corsbuffer;
    
    glm::mat4 ModelMatrix;
    
    glm::vec4 lightPos;
    float load_scale;
    
    //functions
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
                  std::vector<Vector3f>& Positions,
                  std::vector<Vector3f>& Normals,
                  std::vector<Vector2f>& TexCoords,
                  std::vector<VertexBoneData>& Bones,
                  std::vector<unsigned int>& Indices);
    void LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);
    // void CalcFPS();
    void BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms);
    
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
    unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    Matrix4f GetAbsoluteTransform(const aiNode* pNode);
    void computeRestTransformations();
    aiNode * FindNodeRecursive(aiNode *node, aiString nodeName);
    aiNode* FindNode(const aiScene* scene, aiString nodeName);
    void computeRotations(vector<Matrix4f>& rotations);
    void computeQuaternions(vector<Quaternion>& quaternions);
    void computeTraslations(vector<Matrix4f>& traslations);
    void computeOffsets(vector<Matrix4f>& offsets);
    void computeScalings(vector<Matrix4f>& scales);
    void computeParentTransformation(vector<Matrix4f>& pTransformations);
    void computeQuaternionsByVertex();
    quat AntipodalityAwareAdd(const quat &q1, const quat &q2);
    void CalcInterpolatedRotationQLERP(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void computeRotationsTraslationsQuaternions(vector<Matrix4f>& rotations, vector<Matrix4f>& traslations, vector<Quaternion>& quaternions);
    
//    Skeleton *skel;
};


#endif
