//
//  Mesh.cpp
//  myAnimtion
//
//  Created by David Choqueluque Roman on 3/1/19.
//  Copyright © 2019 David Choqueluque Roman. All rights reserved.
//

#include "Mesh.hpp"

Mesh_Skinning::Mesh_Skinning()
{
    VAO = 0;
    memset(Buffers, 0, sizeof(Buffers));
    NumBones = 0;
    pScene = NULL;
}


Mesh_Skinning::~Mesh_Skinning()
{
    Clear();
}


void Mesh_Skinning::Clear()
{
//    for (uint i = 0 ; i < Textures.size() ; i++) {
//        if (Textures[i]) { delete Textures[i]; Textures[i] = NULL; }
//    }
    
    if (Buffers[0] != 0) {
        glDeleteBuffers(sizeof(Buffers)/sizeof(Buffers[0]), Buffers);
    }
    
    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
        VAO = 0;
    }
}

bool Mesh_Skinning::load(const string& file){
    Clear();
    
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    glGenBuffers(num_Buffers,Buffers);
    bool Ret = false;
    pScene = Importer.ReadFile(file.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if(pScene){
        GlobalInverseTransform = pScene->mRootNode->mTransformation;
        GlobalInverseTransform.Inverse();
        Ret =  load_from_scene();
    }
    else{
        cout<<"Error Assimp parser ..."<<endl;
    }
    glBindVertexArray(0);
    return Ret;
}

bool Mesh_Skinning::load_from_scene(){
    meshes.resize(pScene->mNumMeshes);
    
    unsigned int NumVertices = 0;
    unsigned int NumIndices = 0;
    
    vector<Vector3f> Positions;
    vector<Vector3f> Normals;
    vector<Vector2f> TexCoords;
    vector<VertexBoneData> Bones;
    vector<uint> Indices;
    /* Count the number of vertices and indices */
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        meshes[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
        meshes[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
        meshes[i].BaseVertex = NumVertices;
        meshes[i].BaseIndex = NumIndices;
        
        NumVertices += pScene->mMeshes[i]->mNumVertices;
        NumIndices += meshes[i].NumIndices;
    }
    
    // Reserve space in the vectors for the vertex attributes and indices
    Positions.reserve(NumVertices);
    Normals.reserve(NumVertices);
    TexCoords.reserve(NumVertices);
    Bones.resize(NumVertices);
    Indices.reserve(NumIndices);
    
    for (unsigned int i = 0; i < meshes.size(); ++i)
    {
        /* get mesh */
        const aiMesh* paiMesh = pScene->mMeshes[i];
        /* init the mesh */
        InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
    }
    
    // Generate and populate the buffers with vertex attributes and the indices
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[POS_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[TEXCOORD_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[NORMAL_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    glBindBuffer(GL_ARRAY_BUFFER, Buffers[BONE_VB]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffers[INDEX_BUFFER]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);
    return glGetError() == GL_NO_ERROR;
}

void Mesh_Skinning::InitMesh(unsigned int MeshIndex,
                        const aiMesh* paiMesh,
                        std::vector<Vector3f>& Positions,
                        std::vector<Vector3f>& Normals,
                        std::vector<Vector2f>& TexCoords,
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
        //         Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        Positions.push_back(Vector3f(pPos->x, pPos->y, pPos->z));
        
        if (paiMesh->HasNormals())
        {
            const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
            Normals.push_back(Vector3f(pNormal->x, pNormal->y, pNormal->z));
        }
        TexCoords.push_back(Vector2f(pTexCoord->x, pTexCoord->y));
    }
    
    /* Load bones */
//    LoadBones(MeshIndex, paiMesh, Bones);
    
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

void Mesh_Skinning::render()
{
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
    // Set our "myTextureSampler"
    
    glBindVertexArray(VAO);
    
    for (uint i = 0 ; i < meshes.size() ; i++) {
//        const uint MaterialIndex = meshes[i].MaterialIndex;
        
//        assert(MaterialIndex < m_Textures.size());
        
//        if (m_Textures[MaterialIndex]) {
//            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
//        }
        
        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 meshes[i].NumIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(uint) * meshes[i].BaseIndex),
                                 meshes[i].BaseVertex);
    }
    
    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
}
