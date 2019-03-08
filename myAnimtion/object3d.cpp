

//#include <GL/glew.h>


#include <glm/gtc/matrix_transform.hpp>




#include "object3d.h"
//#include "skeleton.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"

#include "OCoR.hpp"
#include "sk_class/utils.h"



                           
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
    pScene = NULL;
    m_NumBones =0;
    meshFilename = filename;
    load_scale=scale;
    loadMesh();
    
}

Object3D::~Object3D(){
    
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &uvbuffer);
    glDeleteBuffers(1, &normalbuffer);
//    glDeleteBuffers(1, &bonebuffer);
//    glDeleteBuffers(1, &indexbuffer);
//    glDeleteBuffers(1, &corsbuffer);
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
        auto start = std::chrono::high_resolution_clock::now();
        CoRs =  ComputeOptimizedCoRs(vertices, indices,bonesInfo, bones, cors);
        auto finish = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Elapsed time: " << elapsed.count() << " s\n";
//        save_CoR("jajaja", cors);
//        cout<<"cors.size: "<<cors.size()<<endl;
//        for (int i=0; i<cors.size(); i++) {
//            cout<<"["<<cors[i].x<<", "<<cors[i].y<<","<<cors[i].z<<"]"<<endl;
//        }
//        computeRestTransformations();
//        for(const BoneInfo& b:bonesInfo){
//            cout<<"_______________Bone Rest Transform______________"<<endl;
//            b.restTransformation.Print();
//        }
//        myLoadAssImp(meshFilename.c_str(),    indices ,vertices, uvs, normals,bones);
    }
    else{
        //Si no consiguen a compilar con el SDK FBX
//        loadOBJ(meshFilename.c_str(), vertices, uvs, normals);
    }
    
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

    glGenBuffers(1, &bonebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
    glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(VertexBoneData), &bones[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &indexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, indexbuffer);
    glBufferData(GL_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &corsbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, corsbuffer);
    glBufferData(GL_ARRAY_BUFFER, cors.size() * sizeof(glm::vec3), &cors[0], GL_STATIC_DRAW);

    
}
void Object3D::loadTmre(const std::string& fileName){
    // ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
    // const aiScene* pScene = m_Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    pScene = m_Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (pScene)
	{
		/* Get transformation matrix for nodes(vertices relative to boes) */
//        aiMatrix4x4 tp1 = pScene->mRootNode->mTransformation;
//        m_GlobalInverseTransform = aiMatrix4x4ToGlm(tp1);
//        m_GlobalInverseTransform =  glm::inverse(m_GlobalInverseTransform);
		
		// ret = InitFromScene(pScene, fileName);
        m_GlobalInverseTransform = pScene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        
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
        cors.resize(NumVertices);
//        cors.assign(NumVertices, Vector3f(0,0,0));
        
        for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
        {
            /* get mesh */
            const aiMesh* paiMesh = pScene->mMeshes[i];
            /* init the mesh */
            InitMesh(i, paiMesh, vertices, normals, uvs, bones, indices);
        }
	}
    else
	{
		std::cout << "Error parsing : " << fileName << " : " << m_Importer.GetErrorString() << std::endl;
	}   
}
void Object3D::InitMesh(unsigned int MeshIndex,
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
         Positions.push_back(Vector3f(pPos->x, pPos->y, pPos->z));
//        Positions.push_back(Vector3f(pPos->x, pPos->z, -(pPos->y)));
//        Positions.push_back(glm::vec3(pPos->x, (pPos->z), -(pPos->y)))  ;
        
        if (paiMesh->HasNormals())
        {
            const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
            Normals.push_back(Vector3f(pNormal->x, pNormal->y, pNormal->z));
//            Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
        }
//        TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
        TexCoords.push_back(Vector2f(pTexCoord->x, pTexCoord->y));
    }
    
    /* Load bones */
    LoadBones(MeshIndex, paiMesh, Bones);
    
    std::cout << "Num faces = " << paiMesh->mNumFaces << std::endl;
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
void Object3D::computeQuaternionsByVertex(){
    unsigned int num_vertices = bones.size();
    vector<Quaternion> CoR_quaternions;
    CoR_quaternions.assign(num_vertices,Quaternion(0, 0, 0, 0));
    
    for(int i = 0; i<num_vertices;i++){
        if(CoRs.count(i)){
//            for (int j=0; j<NUM_BONES_PER_VERTEX; j++) {
            int id_bone = bones[i].IDs[0];
            BoneInfo bone_info_q0 = bonesInfo[id_bone];
            BoneInfo bone_info_q1 = bonesInfo[id_bone+1];
            BoneInfo bone_info_q2 = bonesInfo[id_bone+2];
            BoneInfo bone_info_q3 = bonesInfo[id_bone+3];
            quat q0q1 = AntipodalityAwareAdd(quat(bone_info_q0.quaternion.x,bone_info_q0.quaternion.y,bone_info_q0.quaternion.z,bone_info_q0.quaternion.w)
                                             , quat(bone_info_q1.quaternion.x,bone_info_q1.quaternion.y,bone_info_q1.quaternion.z,bone_info_q1.quaternion.w));
            quat q2q3 = AntipodalityAwareAdd(quat(bone_info_q2.quaternion.x,bone_info_q2.quaternion.y,bone_info_q2.quaternion.z,bone_info_q2.quaternion.w)
                                             , quat(bone_info_q3.quaternion.x,bone_info_q3.quaternion.y,bone_info_q3.quaternion.z,bone_info_q3.quaternion.w));
            
            quat Q = AntipodalityAwareAdd(q0q1, q2q3);
            CoR_quaternions [i] = Quaternion(Q.x, Q.y, Q.z, Q.w);
//            }
        }
    }
}

quat Object3D::AntipodalityAwareAdd(const quat &q1, const quat &q2)
{
    if (glm::dot(q1,q2) >= 0) {
        return q1 + q2;
    } else {
        return q1 + (-q2);
    }
}

void Object3D::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
    std::cout << "Bones for Mesh: " << MeshIndex <<"->"<<pMesh->mNumBones << std::endl;
    
    /* Load bones one by one */
    
    for (unsigned int i = 0; i < pMesh->mNumBones; ++i)
    {
        unsigned int BoneIndex = 0;
        string BoneName(pMesh->mBones[i]->mName.data);
        
        if (m_BoneMapping.find(BoneName) == m_BoneMapping.end())
        {
            /* allocate an index for the new bone */
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            bonesInfo.push_back(bi);
            
//            aiMatrix4x4 tp1 = pMesh->mBones[i]->mOffsetMatrix;
//            bonesInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(tp1);
            bonesInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
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

    // 4th attribute buffer : bone id
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
    glVertexAttribPointer(
                          3,                                // attribute
                          4,                                // size
                          GL_INT,                         // type
                          GL_FALSE,                         // normalized?
                          sizeof(VertexBoneData),                                // stride
                          (void*)0                          // array buffer offset
                          );                          
    
    // 4th attribute buffer : weight id
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
    glVertexAttribPointer(
                          4,                                // attribute
                          4,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          sizeof(VertexBoneData),                                // stride
                          (void*)16                         // array buffer offset
                          );
    
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, corsbuffer);
    glVertexAttribPointer(
                          5,                  // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexbuffer);
    // Draw the triangles !
//    glDrawArrays(GL_TRIANGLES, 0, vertices.size() );
    for (uint i = 0 ; i < meshes.size() ; i++) {
        glDrawElementsBaseVertex(GL_TRIANGLES,
                                 meshes[i].NumIndices,
                                 GL_UNSIGNED_INT,
                                 (void*)(sizeof(uint) * meshes[i].BaseIndex),
                                 meshes[i].BaseVertex);
    }
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);
    
}

aiNode * Object3D::FindNodeRecursive(aiNode *node, aiString nodeName)
{
    if (node->mName == nodeName) {
        return node;
    }
    
    for (int i=0; i<node->mNumChildren; i++) {
        auto child = node->mChildren[i];
        auto foundNode = FindNodeRecursive(child, nodeName);
        if (foundNode != nullptr) {
            return foundNode;
        }
    }
    
    return nullptr;
}

aiNode* Object3D::FindNode(const aiScene* scene, aiString nodeName)
{
    return FindNodeRecursive(scene->mRootNode, nodeName);
}
void Object3D::computeRotations(vector<Matrix4f>& rotations){
    rotations.resize(bonesInfo.size());

}
void Object3D::computeRotationsTraslationsQuaternions(vector<Matrix4f>& rotations, vector<Matrix4f>& traslations, vector<Quaternion>& quaternions){
    rotations.resize(bonesInfo.size());
    traslations.resize(bonesInfo.size());
    quaternions.resize(bonesInfo.size());
    
    for(int i=0;i<rotations.size();i++){
        glm::mat4 TTT (bonesInfo[i].FinalTransformation.m[0][0],bonesInfo[i].FinalTransformation.m[0][1],bonesInfo[i].FinalTransformation.m[0][2],bonesInfo[i].FinalTransformation.m[0][3],
                  bonesInfo[i].FinalTransformation.m[1][0],bonesInfo[i].FinalTransformation.m[1][1],bonesInfo[i].FinalTransformation.m[1][2],bonesInfo[i].FinalTransformation.m[1][3],
                  bonesInfo[i].FinalTransformation.m[2][0],bonesInfo[i].FinalTransformation.m[2][1],bonesInfo[i].FinalTransformation.m[2][2],bonesInfo[i].FinalTransformation.m[2][3],
                  bonesInfo[i].FinalTransformation.m[3][0],bonesInfo[i].FinalTransformation.m[3][1],bonesInfo[i].FinalTransformation.m[3][2],bonesInfo[i].FinalTransformation.m[3][3]);
        
        glm::vec3 skew;
        vec4 perspective;
        vec3 translateOut;
        quat rotateOut;
        vec3 scaleOut;
        
        glm::decompose(TTT, scaleOut, rotateOut, translateOut, skew, perspective);
        mat4 rot = glm::toMat4(rotateOut);
        Matrix4f rR;
//        rR.m[0][0] = rot[0][0]; rR.m[0][1] = rot[0][1]; rR.m[0][2] = rot[0][2]; rR.m[0][3] = rot[0][3];
//        rR.m[0][0] = rot[1][0]; rR.m[1][1] = rot[1][1]; rR.m[1][2] = rot[1][2]; rR.m[1][3] = rot[1][3];
//        rR.m[0][0] = rot[2][0]; rR.m[2][1] = rot[2][1]; rR.m[2][2] = rot[2][2]; rR.m[2][3] = rot[2][3];
//        rR.m[0][0] = rot[3][0]; rR.m[3][1] = rot[3][1]; rR.m[3][2] = rot[3][2]; rR.m[3][3] = rot[3][3];
        rR.InitRotateTransform(Quaternion(rotateOut.x, rotateOut.y, rotateOut.z, rotateOut.w));
        rotations[i] = rR;
        Matrix4f tT;
        tT.InitTranslationTransform(translateOut.x, translateOut.y, translateOut.z);
        traslations[i] = tT;
        quaternions[i] = Quaternion(rotateOut.x, rotateOut.y, rotateOut.z, rotateOut.w);
        //string boneName(it.first);
        //const aiNode* bone_node = FindNode(pScene, aiString(boneName));
    }
}
void Object3D::computeQuaternions(vector<Quaternion>& quaternions){
    quaternions.resize(bonesInfo.size());
    
    for(int i=0;i<quaternions.size();i++){
//        quaternions[i] = bonesInfo[i].quaternion;
        
        //string boneName(it.first);
        //const aiNode* bone_node = FindNode(pScene, aiString(boneName));
    }
}
void Object3D::computeTraslations(vector<Matrix4f>& traslations){
    traslations.resize(bonesInfo.size());
    
    for(int i=0;i<traslations.size();i++){
//        traslations[i] = bonesInfo[i].traslation;
        
        //string boneName(it.first);
        //const aiNode* bone_node = FindNode(pScene, aiString(boneName));
    }
}
void Object3D::computeOffsets(vector<Matrix4f>& offsets){
    offsets.resize(bonesInfo.size());
    
    for(int i=0;i<offsets.size();i++){
        offsets[i] = bonesInfo[i].BoneOffset;
//        cout<<"................ Ofeset"<<endl;
//        offsets[i].printMatrix4f();
        
        //string boneName(it.first);
        //const aiNode* bone_node = FindNode(pScene, aiString(boneName));
    }
}
void Object3D::computeScalings(vector<Matrix4f>& scales){
    scales.resize(bonesInfo.size());
    
    for(int i=0;i<scales.size();i++){
//        scales[i] = bonesInfo[i].scale;
        //        cout<<"................ Ofeset"<<endl;
        //        offsets[i].printMatrix4f();
        
        //string boneName(it.first);
        //const aiNode* bone_node = FindNode(pScene, aiString(boneName));
    }
}
void Object3D::computeParentTransformation(vector<Matrix4f>& pTransformations){
    pTransformations.resize(bonesInfo.size());
    
    for(int i=0;i<pTransformations.size();i++){
//        pTransformations[i] = bonesInfo[i].ParentTransform;
        //        cout<<"................ Ofeset"<<endl;
        //        offsets[i].printMatrix4f();
        
        //string boneName(it.first);
        //const aiNode* bone_node = FindNode(pScene, aiString(boneName));
    }
}
void Object3D::computeRestTransformations()
{
//    if (pScene->mNumAnimations == 0) {
//        cout<<"No animation in model loaded...."<<endl;
//        return;
//    }
    
    for(const auto it: m_BoneMapping){
        
        string boneName(it.first);
        const aiNode* bone_node = FindNode(pScene, aiString(boneName));
        if(bone_node){
//            cout<<bone_node->mName.data<<endl;
            Matrix4f trans = GetAbsoluteTransform(bone_node);
//            trans.printMatrix4f();
//            bonesInfo[m_BoneMapping[bone_node->mName.data]].restTransformation = trans;
//            bonesInfo[m_BoneMapping[bone_node->mName.data]].TRANSF_REST = Transform(trans.toMat4());
        }
        else{
            cout<<"Bone NOT FOUND!!!!!"<<endl;
        }
    }
    
}


Matrix4f Object3D::GetAbsoluteTransform(const aiNode* pNode)
{
    //string NodeName(pNode->mName.data);
    
    std::deque<Matrix4f> transformStack;
    
    aiString currentJointName = pNode->mName;
//    aiNode* node;
    while(true)
    {
        //const auto& jParentRestTransform = GetJoint(currentJointName).GetParentRelativeRestTransform(); //mtransformation
        aiNode* node = FindNode(pScene,currentJointName);
        if(node){
            Matrix4f NodeTransformation(node->mTransformation);
//            cout<<"mmmmmmm "<<endl;
            transformStack.push_front(NodeTransformation);
//            NodeTransformation.printMatrix4f();
            if (!(node->mParent)) {
                break;
            }
            currentJointName = node->mParent->mName;
        }
        else{
            cout<<"parent NOT FOUND!!!!!"<<endl;
            break;
        }
    }
    
    Matrix4f absoluteTransform;
    absoluteTransform.InitIdentity();
    
    for (const Matrix4f& t: transformStack) {
        absoluteTransform = absoluteTransform * t;
    }
    return absoluteTransform;
}

void Object3D::BoneTransform(float TimeInSeconds,std::vector<Matrix4f>& Transforms)
{
    if (pScene->mNumAnimations == 0) {
        cout<<"No animation in model loaded...."<<endl;
        return;
    }
    cout<<"Exist animation-numChanels: "<<pScene->mAnimations[0]->mNumChannels<<endl;
    
     Matrix4f Identity;
     Identity.InitIdentity();
//    glm::mat4 Identity = glm::mat4(1);

    unsigned int numPosKeys = pScene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
    animDuration = pScene->mAnimations[0]->mChannels[0]->mPositionKeys[numPosKeys - 1].mTime;

    float TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, animDuration);


//     float TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
//     float TimeInTicks = TimeInSeconds * TicksPerSecond;
//     float AnimationTime = fmod(TimeInTicks, (float)pScene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(AnimationTime, pScene->mRootNode, Identity);

    Transforms.resize(m_NumBones);

    for (uint i = 0 ; i < m_NumBones ; i++) {
        Transforms[i] = bonesInfo[i].FinalTransformation;
        
//        rotations[i] = bonesInfo[i].TRANSF_POSE.GetRotation()*glm::inverse(bonesInfo[i].TRANSF_REST.GetRotation());
//        traslations_rest[i] = bonesInfo[i].TRANSF_POSE.GetTranslation();
//        traslations_pose[i] = bonesInfo[i].TRANSF_POSE.GetTranslation();

    }
}

void Object3D::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform)
{
	string NodeName(pNode->mName.data); //aiNode

	const aiAnimation* pAnimation = pScene->mAnimations[0];

//    aiMatrix4x4 tp1 = pNode->mTransformation;
    Matrix4f NodeTransformation(pNode->mTransformation);
//    glm::mat4 NodeTransformation = aiMatrix4x4ToGlm(tp1);
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    
//    Quaternion quaternion(0,0,0,0);
    

	if (pNodeAnim) {
        Matrix4f RotationM;
        Matrix4f TranslationM;
        Matrix4f ScalingM;
//        cout<<"*********+ NodeName: "<<pNodeAnim->mNodeName.data<<std::endl;
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
        
//        ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));
//        cout<<"========= ScalingM ========="<<std::endl;
//        ScalingM.printMatrix4f();
//        util_printGLMMat4(ScalingM);


        
        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
//        CalcInterpolatedRotationQLERP(RotationQ, AnimationTime, pNodeAnim);
        RotationM = Matrix4f(RotationQ.GetMatrix());
        
//        quaternion = Quaternion(RotationQ.x,RotationQ.y,RotationQ.z,RotationQ.w);
        
//        RotationM.
//        cout<<"========= RotationM ========="<<std::endl;
//        RotationM.printMatrix4f();
//        
//        aiMatrix3x3 tp = RotationQ.GetMatrix();
//        glm::mat4 RotationM = aiMatrix3x3ToGlm(tp);
//        cout<<"========= RotationM ========="<<std::endl;
//        util_printGLMMat4(RotationM);

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
//        cout<<"========= TranslationM ========="<<std::endl;
//        TranslationM.printMatrix4f();
//        glm::mat4 TranslationM;
//        TranslationM = glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z));
//         std::cout<<"========= TraslationM ========="<<std::endl;
//         util_printGLMMat4(TranslationM);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
//        std::cout<<"========= NodeTransformation ========="<<std::endl;
//        NodeTransformation.printMatrix4f();
	}

//    glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;
    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		unsigned int BoneIndex = m_BoneMapping[NodeName];
		bonesInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * bonesInfo[BoneIndex].BoneOffset;
//        if (pNodeAnim){
//            bonesInfo[BoneIndex].quaternion = quaternion;
//            bonesInfo[BoneIndex].Rotation = RotationM;
//            bonesInfo[BoneIndex].traslation = TranslationM;
//            bonesInfo[BoneIndex].scale = ScalingM;
//            bonesInfo[BoneIndex].ParentTransform = ParentTransform;
//        }
        
        //bonesInfo[BoneIndex].TRANSF_POSE = Transform(bonesInfo[BoneIndex].FinalTransformation.toMat4());
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}
//Matrix4f computeTraslation(Matrix4f R, Matrix4f t, ){
//
//}

/*Find aiNode on aiAnimation::Channels  each channel is a aiNodeAnim*/
const aiNodeAnim* Object3D::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            
			return pNodeAnim;
		}
	}

	return NULL;
}

/*Find the index of the next time  on aiNodeAnim::Positions*/
unsigned int Object3D::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) 
	{
		
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            // std::cout<<"-----> FindPosition: "<<i<<std::endl;
			return i;
		}
	}
	
	assert(0);
	return 0;
}
/*Find the index of the next time  on aiNodeAnim::Positions*/
unsigned int Object3D::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            // std::cout<<"-----> FindRotation: "<<i<<std::endl;
			return i;
		}
	}

	assert(0);
	return 0;
}
/*Find the index of the next time  on aiNodeAnim::Positions*/
unsigned int Object3D::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            // std::cout<<"-----> FindScaling: "<<i<<std::endl;
			return i;
		}
	}

	assert(0);
	return 0;
}

void Object3D::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }
            
    uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
//    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    
    aiVector3D Delta = End - Start;
    
    Out = Start + Factor * Delta;
    // std::cout<<"========= CalcInterpolatedPosition ========="<<Out.x<<","<<Out.y<<","<<Out.z<<std::endl;
//    std::cout<<"========= CalcInterpolatedPosition: Start ========="<<Start.x<<","<<Start.y<<","<<Start.z<<std::endl;
//    std::cout<<"========= CalcInterpolatedPosition: End ========="<<End.x<<","<<End.y<<","<<End.z<<std::endl;
}


void Object3D::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
//    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;    
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void Object3D::CalcInterpolatedRotationQLERP(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }
    
    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    
//    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
//    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
//    //    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    quat a;
    a.x = StartRotationQ.x;
    a.y = StartRotationQ.y;
    a.z = StartRotationQ.z;
    a.w = StartRotationQ.w;
    quat b;
    b.x = EndRotationQ.x;
    b.y = EndRotationQ.y;
    b.z = EndRotationQ.z;
    b.w = EndRotationQ.w;
    quat r = AntipodalityAwareAdd(a, b);
//    glm::normalize(r);
//    Out = aiQuaternion(r.x,r.y,r.z,r.w);
    Out.x=r.x;
    Out.y=r.y;
    Out.z=r.z;
    Out.w=r.w;
//    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}



void Object3D::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
//    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
    
//    cout<<"Start: "<<(double)Start.x<<","<<(double)Start.y<<","<<(double)Start.z<<endl;
//    cout<<"End: "<<(double)End.x<<","<<(double)End.y<<","<<(double)End.z<<endl;
//    cout<<"Delta: "<<Delta.x<<","<<Delta.y<<","<<Delta.z<<endl;
//    cout<<"Out: "<<Out.x<<","<<Out.y<<","<<Out.z<<endl;
}

