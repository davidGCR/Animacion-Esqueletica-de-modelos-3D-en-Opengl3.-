

#include <GL/glew.h>


#include <glm/gtc/matrix_transform.hpp>


#include <iostream>

#include "object3d.h"

#include "skeleton.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"




glm::mat3 aiMatrix3x3ToGlm(const aiMatrix3x3 &from);
glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from);
                           
Object3D::Object3D(){
    glGenVertexArrays(1, &VertexArrayID);
    Texture=0;
    ModelMatrix = glm::mat4(1.0);
    lightPos=glm::vec4(4,4,1,0);
    m_NumBones =0;
}

Object3D::Object3D(std::string filename, bool flac,float scale){
    glGenVertexArrays(1, &VertexArrayID);
    Texture=0;
    ModelMatrix = glm::mat4(1.0);
    lightPos=glm::vec4(4,4,1,0);
    flac_lib = flac;
    
    m_NumBones =0;
    meshFilename = filename;
    load_scale=scale;
    loadMesh();
    
}

Object3D::~Object3D(){
    
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);
//    delete skel;
    
    
}

void Object3D::setShaders(std::string vfile,std::string ffile){
    
    
    programID = LoadShaders( vfile.c_str(), ffile.c_str());
    
    // Get a handle for our "MVP" uniform
    MatrixID = glGetUniformLocation(programID, "MVP");
    ViewMatrixID = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    
}

void Object3D::loadMesh(){
    
//    skel = new Skeleton();
    
    // loadFBX(meshFilename.c_str(), vertices, uvs, normals,skel, load_scale);
    
    
    if(flac_lib)
    {
        
        // loadAssImp(meshFilename.c_str(), indices ,vertices, uvs, normals);
        loadTmre(meshFilename.c_str());
//        myLoadAssImp(meshFilename.c_str(),    indices ,vertices, uvs, normals,bones);
    }
    else{
        //Si no consiguen a compilar con el SDK FBX
        loadOBJ(meshFilename.c_str(), vertices, uvs, normals);
    }
    
    //skel->printSkeleton();
    /*skel->skeletonMesh();
     skel->skeletonVBO();
     skel->setShaders("gizmo.vertexshader", "gizmo.fragmentshader");
     std::cout << "finish loading" << std::endl;*/
    
    glBindVertexArray(VertexArrayID);
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &normalbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    
}
void Object3D::loadTmre(const std::string& fileName){
    Assimp::Importer m_Importer;
    
    const aiScene* pScene = m_Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    
    meshes.resize(pScene->mNumMeshes);
    
    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;
    
    /* Count the number of vertices and indices */
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
        meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        meshes[i].BaseVertex = NumVertices;
        meshes[i].BaseIndex = NumIndices;
        
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices += pScene->mMeshes[i]->mNumFaces * 3;
    }
    
    // Reserve space in the vectors for the vertex attributes and indices
    vertices.reserve(NumVertices);
    normals.reserve(NumVertices);
    uvs.reserve(NumVertices);
    bones.resize(NumVertices);
    indices.reserve(NumIndices);
    
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        /* get mesh */
        const aiMesh* paiMesh = pScene->mMeshes[i];
        /* init the mesh */
        InitMesh(i, paiMesh, vertices, normals, uvs, bones, indices);
    }
}
void Object3D::InitMesh(unsigned int MeshIndex,
                           const aiMesh* paiMesh,
                           std::vector<glm::vec3>& Positions,
                           std::vector<glm::vec3>& Normals,
                           std::vector<glm::vec2>& TexCoords,
                           std::vector<VertexBoneData>& Bones,
                           std::vector<unsigned int>& Indices)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    
    std::cout << "Num vertices = " << paiMesh->mNumVertices << std::endl;
    /* Populize the vertex attribute vectors */
    for (unsigned int i = 0; i < paiMesh->mNumVertices; ++i)
    {
        /* Get pos normal texCoord */
        
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
        
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
        
        /* store pos normal texCoord */
        Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        
        if (paiMesh->HasNormals())
        {
            const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
            Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
        }
        TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
    }
    
    /* Load bones */
    LoadBones(MeshIndex, paiMesh, Bones);
    
    /* Populate the index buffer */
    for (unsigned int i = 0; i < paiMesh->mNumFaces; ++i)
    {
        const aiFace& Face = paiMesh->mFaces[i];
        /* if mNumIndices != 3 then worning */
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
}

void Object3D::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
    std::cout << "Bones for Mesh: " << MeshIndex <<"->"<<pMesh->mNumBones << std::endl;
    
    /* Load bones one by one */
    for (unsigned int i = 0; i < pMesh->mNumBones; ++i)
    {
        unsigned int BoneIndex = 0;
        std::string BoneName(pMesh->mBones[i]->mName.data);
        
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
        {
            /* allocate an index for the new bone */
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            bonesInfo.push_back(bi);
            
            aiMatrix4x4 tp1 = pMesh->mBones[i]->mOffsetMatrix;
            bonesInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(tp1);
            m_BoneMapping[BoneName] = BoneIndex;
        }
        else
        {
            BoneIndex = m_BoneMapping[BoneName];
        }
        //save weights
        for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; ++j)
        {
            //std::cout << pMesh->mBones[i]->mWeights. << std::endl;
            unsigned int VertexID = meshes[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}



bool Object3D::myLoadAssImp( const char * path, std::vector<unsigned int> & indices, std::vector<glm::vec3> & vertices,
                            std::vector<glm::vec2> & uvs, std::vector<glm::vec3> & normals, std::vector<VertexBoneData>& bones)
{
    std::cout<<"loading with my Assimp..."<< path<<std::endl;
    Assimp::Importer importer;
    
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
    // const aiScene* scene = importer.ReadFile(path, 0);
    if( !scene) {
        fprintf( stderr, importer.GetErrorString());
        getchar();
        return false;
    }
    int total_vertices = 0;
    
    
    
    for(unsigned int m = 0; m < scene->mNumMeshes; m++)
    {
        aiMesh* mesh = scene->mMeshes[m]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)
        total_vertices += mesh->mNumVertices;
        // processNode( scene->mRootNode, scene, mesh);
        
    }
    aiMesh* mesh = scene->mMeshes[0];
//    aiMesh* mesh1 = scene->mMeshes[1];
    
    vertices.reserve(total_vertices);
    uvs.reserve(total_vertices);
    normals.reserve(total_vertices);
    indices.reserve(3*mesh->mNumFaces);
    std::cout<<"total_mallas: "<<scene->mNumMeshes<<std::endl;
    std::cout<<"total_vertices: "<<total_vertices<<","<<mesh->mNumVertices<<std::endl;
    std::cout<<"total_bones: "<<mesh->mNumBones<<std::endl;
    std::cout<<"total_feces: "<<mesh->mNumFaces<<std::endl;
    
//    std::cout<<"total_bones(1): "<<mesh1->mNumBones<<std::endl;
    
    // // Fill vertices positions
     for(unsigned int i=0; i<total_vertices; i++){
         aiVector3D pos = mesh->mVertices[i];
         vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
         // cout<<pos.x<<","<<pos.y<<","<<pos.z<<endl;
     }
    
     // // Fill vertices texture coordinates
//     for(unsigned int i=0; i<total_vertices; i++){
//         aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
//         uvs.push_back(glm::vec2(UVW.x, UVW.y));
//     }
//
//     // // Fill vertices normals
//     for(unsigned int i=0; i<total_vertices; i++){
//         aiVector3D n = mesh->mNormals[i];
//         normals.push_back(glm::vec3(n.x, n.y, n.z));
//     }
//
    
     // // Fill face indices
     for (unsigned int i=0; i<3*(mesh->mNumFaces); i++){
         // Assume the model has only triangles.
         indices.push_back(mesh->mFaces[i].mIndices[0]);
         indices.push_back(mesh->mFaces[i].mIndices[1]);
         indices.push_back(mesh->mFaces[i].mIndices[2]);
     }
    
    //// Fill bones
    // bones.reserve(mesh->mNumVertices);
    // for(unsigned int i = 0; i < mesh->mNumBones; i++)
    // {
    //     std::cout<<mesh->mBones[i]->mNumWeights<<std::endl;
    //     if(mesh->mBones[i]->mNumWeights > NUM_BONES_PER_VERTEX){
    //         std::cout<<"========= muchos bones aqui: "<<mesh->mBones[i]->mNumWeights<<std::endl;
    //     }
    //     // for(unsigned int b=0;mesh->mBones[i];b++){
    
    //     // }
    // }
    
    // The "scene" pointer will be deleted automatically by "importer"
    return true;
}


void Object3D::draw(){
    
    //skel->ModelMatrix = glm::translate(ModelMatrix,glm::vec3(0,0,1));
    //skel->draw();
    
    glUseProgram(programID);
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    
    glm::mat4 lrot = glm::rotate(glm::mat4(1.0),0.0f,glm::vec3(0,1,0));
    
    lightPos =  lrot * lightPos;
    
    static int factor= 0.0;
    lightPos.x += 0.1*factor;
    
    if(lightPos.x>7.0){
        factor=-1;
    }
    else if(lightPos.x<-7.0){
        factor=1;
    }
    
    
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(TextureID, 0);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
                          1,                                // attribute
                          2,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
                          2,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() );
    // glDrawArrays(GL_LINE_LOOP, 0, vertices.size() );
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
}

void Object3D::draw_assimp(){
    
    //skel->ModelMatrix = glm::translate(ModelMatrix,glm::vec3(0,0,1));
    //skel->draw();
    
    glUseProgram(programID);
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    
    glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
    
    // Send our transformation to the currently bound shader,
    // in the "MVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    
    glm::mat4 lrot = glm::rotate(glm::mat4(1.0),0.0f,glm::vec3(0,1,0));
    
    lightPos =  lrot * lightPos;
    
    static int factor= 0.0;
    lightPos.x += 0.1*factor;
    
    if(lightPos.x>7.0){
        factor=-1;
    }
    else if(lightPos.x<-7.0){
        factor=1;
    }
    
    
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(TextureID, 0);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glVertexAttribPointer(
                          1,                                // attribute
                          2,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
                          2,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    
    // glEnableVertexAttribArray(3);
    // glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, EBO );
    // glBufferData( GL_ELEMENT_ARRAY_BUFFER, this->indices.size( ) * sizeof( GLuint ), &this->indices[0], GL_STATIC_DRAW );
    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() );
    // glDrawArrays(GL_LINE_LOOP, 0, vertices.size() );
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
    // glDisableVertexAttribArray(3);
    
}


glm::mat3 aiMatrix3x3ToGlm(const aiMatrix3x3 &from)
{
    glm::mat3 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2;    to[2][0] = from.a3;
    to[0][1] = from.b1; to[1][1] = from.b2;    to[2][1] = from.b3;
    to[0][2] = from.c1; to[1][2] = from.c2;    to[2][2] = from.c3;
    
    return to;
}

glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &from)
{
    glm::mat4 to;
    //the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    to[0][0] = from.a1; to[1][0] = from.a2;    to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2;    to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2;    to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2;    to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}
