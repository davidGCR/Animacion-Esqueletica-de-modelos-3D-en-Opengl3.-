

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

    glGenBuffers(1, &bonebuffer);
    glBindBuffer(GL_ARRAY_BUFFER, bonebuffer);
    glBufferData(GL_ARRAY_BUFFER, bones.size() * sizeof(VertexBoneData), &bones[0], GL_STATIC_DRAW);

    
}
void Object3D::loadTmre(const std::string& fileName){
    Assimp::Importer m_Importer;
    // ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)
    // const aiScene* pScene = m_Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    pScene = m_Importer.ReadFile(fileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
    
    if (pScene)
	{
		/* Get transformation matrix for nodes(vertices relative to boes) */
		aiMatrix4x4 tp1 = pScene->mRootNode->mTransformation;
		m_GlobalInverseTransform = aiMatrix4x4ToGlm(tp1);
		m_GlobalInverseTransform =  glm::inverse(m_GlobalInverseTransform);
		
		// ret = InitFromScene(pScene, fileName);
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
    else
	{
		std::cout << "Error parsing : " << fileName << " : " << m_Importer.GetErrorString() << std::endl;
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
        // Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
        Positions.push_back(glm::vec3(pPos->x, (pPos->z), -(pPos->y)));
        
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

    
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, vertices.size() );
    // glDrawArrays(GL_LINE_LOOP, 0, vertices.size() );
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    
}


void Object3D::BoneTransform(float TimeInSeconds,std::vector<glm::mat4>& Transforms)
{
    // Matrix4f Identity;
    // Identity.InitIdentity();
    glm::mat4 Identity = glm::mat4();
    if(pScene){
        std::cout<<"Dbug scene: "<<pScene->mAnimations[10]->mNumChannels<<std::endl;
        // unsigned int numPosKeys = pScene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
    }
    else{
        std::cout<<"No scene..."<<std::endl;
    }

    // unsigned int numPosKeys = pScene->mAnimations[0]->mChannels[0]->mNumPositionKeys;
	// animDuration = pScene->mAnimations[0]->mChannels[0]->mPositionKeys[numPosKeys - 1].mTime;
    
    // float TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
    // float TimeInTicks = TimeInSeconds * TicksPerSecond;
    // float AnimationTime = fmod(TimeInTicks, animDuration);

    // std::cout<<"animDuration: "<<animDuration<<std::endl;
    // std::cout<<", TicksPerSecond: "<<TicksPer<<std::endl;
    // std::cout<<", TimeInTicks: "<<TimeInTicks<<std::endl;

    // ReadNodeHeirarchy(AnimationTime, pScene->mRootNode, Identity);

    // Transforms.resize(m_NumBones);

    // for (uint i = 0 ; i < m_NumBones ; i++) {
    //     Transforms[i] = bonesInfo[i].FinalTransformation;
    // }
}

void Object3D::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
{
	std::string NodeName(pNode->mName.data); //aiNode

	const aiAnimation* pAnimation = pScene->mAnimations[0];

	aiMatrix4x4 tp1 = pNode->mTransformation;
	glm::mat4 NodeTransformation = aiMatrix4x4ToGlm(tp1);
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		glm::mat4 ScalingM;
		
		ScalingM = glm::scale(ScalingM, glm::vec3(Scaling.x, Scaling.y, Scaling.z));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
		aiMatrix3x3 tp = RotationQ.GetMatrix();
		glm::mat4 RotationM = aiMatrix3x3ToGlm(tp);

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		glm::mat4 TranslationM;
		TranslationM = glm::translate(TranslationM, glm::vec3(Translation.x, Translation.y, Translation.z));

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM *ScalingM;
	}

	glm::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
		unsigned int BoneIndex = m_BoneMapping[NodeName];
		bonesInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * bonesInfo[BoneIndex].BoneOffset;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

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
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
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
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;    
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
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
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

