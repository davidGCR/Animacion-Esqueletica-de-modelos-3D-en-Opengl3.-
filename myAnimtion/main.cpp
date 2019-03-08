//
//  main.cpp
//  myAnimtion
//
//  Created by David Choqueluque Roman on 2/22/19.
//  Copyright © 2019 David Choqueluque Roman. All rights reserved.
//
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

// #include <glm/gtx/string_cast.hpp>
using namespace glm;


#include "object3d.h"
#include "gizmo.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "sk_class/constants.h"
#include <sys/time.h>
//#include "OCoR.hpp"
// #include "sk_class/utils.h"

#define INVALID_UNIFORM_LOCATION 0xffffffff
long long GetCurrentTimeMillis();


int m_frameCount = 0;
int m_fps = 0;

long long m_startTime = GetCurrentTimeMillis();
long long m_frameTime = m_startTime;


// print out matrix by rows
void printGLMMat4(glm::mat4  mat){
  int i,j;
  for (j=0; j<4; j++){
    for (i=0; i<4; i++){
    printf("%f ",mat[i][j]);
  }
  printf("\n");
 }
}


void CalcFPS()
{
    m_frameCount++;

    long long time = GetCurrentTimeMillis();
    
    if (time - m_frameTime >= 1000) {
        m_frameTime = time;
        m_fps = m_frameCount;
        m_frameCount = 0;
    }
}
long long GetCurrentTimeMillis()
{
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
}

float GetRunningTime(long long m_startTime)
{
    float RunningTime = (float)((double)GetCurrentTimeMillis() - (double)m_startTime) / 1000.0f;
    return RunningTime;
}

GLint GetUniformLocation(GLint program, const char* pUniformName)
{
    GLuint Location = glGetUniformLocation(program, pUniformName);

    if (Location == INVALID_UNIFORM_LOCATION) {
        fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
    }

    return Location;
}

//void SetBoneTransform(uint Index, const Matrix4f& Transform)
//{
//    assert(Index < MAX_BONES);
//    //Transform.Print();
//    glUniformMatrix4fv(m_boneLocation[Index], 1, GL_TRUE, (const GLfloat*)Transform);
//}

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
//#ifdef __APPLE__
//    // Select OpenGL 4.1
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//#else
//    // Select OpenGL 4.3
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//#endif
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    // Open the window, get current resolution and set fullscreen
//    std::string title = "My Window -- ";
//    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
//    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
//    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
//    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
//
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Open a window and create its OpenGL context


    window = glfwCreateWindow( 1024, 768, "Phong Shader", NULL, NULL);
//    glfwSetWindowPos(window, 1500, 0);
    
    
//    GLFWwindow* window = glfwCreateWindow(640,480,"hola triangulo",NULL,NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);


    // Dark blue background
    glClearColor(0.125f, 0.152f, 0.16f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    //Si tienes FBX
    // Object3D* body = new Object3D("data/merc/source/merc.fbx",1.0);
    // body->ModelMatrix = glm::rotate(body->ModelMatrix,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
    
    //SINO con OBJ
//    Object3D* body = new Object3D(PATH+"data/batma.obj",true);
//     Object3D* body = new Object3D(PATH+"data/models/gilbert/source/gilbert.fbx",true);
//   Object3D* body = new Object3D(PATH+"data/models/big-guy/source/big_guy.fbx",true);
//    Object3D* body = new Object3D(PATH+"data/models/big-guy2.fbx",true);
//     Object3D* body = new Object3D(PATH+"data/models/model2.dae",true);
//     Object3D* body = new Object3D(PATH+"data/models/Monster_1/Monster_1.dae",true);
//    Object3D* body = new Object3D( PATH+"data/models/simple_animated.dae",true);

//        Object3D* body = new Object3D( PATH+"data/models/ArmatureStraight.dae",true);
//    Object3D* body = new Object3D( PATH+"data/models/cylinder.dae",true);
//    Object3D* body = new Object3D( PATH+"data/models/object/cylinder/leafbone.fbx",true);
//    Object3D* body = new Object3D( PATH+"data/models/cilindro_l.dae",true);
//    Object3D* body = new Object3D( PATH+"data/models/rectangulo_animated.dae",true);
//     Object3D* body = new Object3D( PATH+"data/models/dancer.dae",true);
//     Object3D* body = new Object3D( PATH+"data/models/oso.dae",true);
//     Object3D* body = new Object3D( PATH+"data/models/dinosaurio.dae",true);
     Object3D* body = new Object3D( PATH+"data/models/groo.dae",true);
//    Object3D* body = new Object3D( PATH+"data/models/tortuga.dae",true);

//    Object3D* body = new Object3D( PATH+"data/models/oso.dae",true);
    body->ModelMatrix = glm::rotate(body->ModelMatrix,glm::radians(-90.0f),glm::vec3(1.0f,0.0f,0.0f));
    
    
    cout<<"vertices total: "<<body->vertices.size()<<endl;
    cout<<"indices total: "<<body->indices.size()<<endl;
    cout<<"bonesInfo total: "<<body->bonesInfo.size()<<endl;
    cout<<"bones total: "<<body->bones.size()<<endl;
    map<int,Vector3f> results;
    
    
    //ComputeOptimizedCoRs(body->vertices, body->indices, body->bonesInfo, body->bones, body->cors);
    
//     Object3D* body = new Object3D(PATH+"data/models/boblampclean.md5mesh",true);
//    Object3D* body = new Object3D(PATH+"data/models/old-man-run-animation/source/Corriendo-Old.fbx",true);
//    Object3D* body = new Object3D(PATH+"data/models/village-hero/sword_and_shield_idle.fbx",true);
    //Object3D* trex = new Object3D("data/trex/TrexByJoel3d.fbx");
    //trex->setShaders("StandardShading.vertexshader", "phong.fragmentshader");

    body->setShaders(PATH+"StandardShading.vertexshader.glsl", PATH+"phong.fragmentshader.glsl");

    Gizmo* gizmo = new Gizmo();
    gizmo->setShaders(PATH+"gizmo.vertexshader", PATH+"gizmo.fragmentshader");

    //textuta
    GLuint Texture= loadDDS((PATH+"data/cube/uvmap.DDS").c_str());
    GLuint TextureID  = glGetUniformLocation(gizmo->programID, (PATH+"myTextureSampler").c_str());

    // GLuint textureID;
    // glGenTextures(1,&textureID);
    // glBindTexture(GL_TEXTURE_2D,textureID);
    // glTextureImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_BGR,GL_UNSIGNED_BYTE,data);

    
//    body->BoneTransform(0,Transforms);

    // Cull triangles which normal is not towards the camera
    //glEnable(GL_CULL_FACE);
//    const string name_glob_trans = "globalTransformation";
//    GLuint glob_trasf = glGetUniformLocation(body->programID, name_glob_trans.c_str());
//    glUniformMatrix4fv(glob_trasf, 1,GL_TRUE,(const GLfloat*)body->m_GlobalInverseTransform);
//
//    vector<Matrix4f> offs;
//    body->computeOffsets(offs);
//
//    for (int i=0; i<offs.size(); i++) {
//        const string name_off = "offs[" + to_string(i) + "]";
//        GLuint boneoffset = glGetUniformLocation(body->programID, name_off.c_str());
//        glUniformMatrix4fv(boneoffset, 1, GL_TRUE, (const GLfloat*)Matrix4f(offs[i]) );
//    }
    
    
    
    do{
//        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();

        //////////////////////////////////////////////////////
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);
        ///////////////////////////////////////////////////

        // // //animation
        CalcFPS();
        float t = GetRunningTime(m_startTime);

        cout<<" /////////////////////////////////// time: "<<t<<endl;
        vector<Matrix4f> Transforms;
        vector<glm::quat> rotations00;
        vector<glm::vec3> traslations_rest;
        vector<glm::vec3> traslations_pose;
//        vector<glm::quat> quaternions;
        vector<Quaternion> quaternions;
        vector<Matrix4f> traslations;
        vector<Matrix4f> scales;
        vector<Matrix4f> pTransformations;
        vector<Matrix4f> rotations;
        
        body->BoneTransform(t,Transforms);
//        body->computeRotations(quaternions);
//        body->computeQuaternions(quaternions);
//        body->computeTraslations(traslations);
//        body->computeScalings(scales);
//        body->computeParentTransformation(pTransformations);
//        body->computeRotations(rotations);
        body->computeRotationsTraslationsQuaternions(rotations, traslations,quaternions);
        
        for (uint i = 0 ; i < Transforms.size() ; i++) {
//            SetBoneTransform(i, Transforms[i]);
//            cout<<"__________________________ Transform: "<<i<<endl;
//            Transforms[i].printMatrix4f();
            const string name = "gBones[" + to_string(i) + "]";
            GLuint boneTransform = glGetUniformLocation(body->programID, name.c_str());
            glUniformMatrix4fv(/*m_boneLocation[i]*/boneTransform, 1, GL_TRUE, (const GLfloat*)Transforms[i]);
            
//            const string name_t = "traslations_rest[" + to_string(i) + "]";
//            GLuint traslations_r = glGetUniformLocation(body->programID, name_t.c_str());
//            glUniform3fv(traslations_r, 1, glm::value_ptr(traslations_rest[i]));
//
//            const string name_p = "traslations_pose[" + to_string(i) + "]";
//            GLuint traslations_p = glGetUniformLocation(body->programID, name_p.c_str());
//            glUniform3fv(traslations_p, 1, glm::value_ptr(traslations_pose[i]));
//
//            const string name_rot = "rotations[" + to_string(i) + "]";
//            GLuint bonerotation = glGetUniformLocation(body->programID, name_rot.c_str());
//            glUniformMatrix4fv(bonerotation, 1, GL_TRUE, glm::value_ptr(glm::toMat4(rotations[i])));
            
//            const string name_quad = "quaternions[" + to_string(i) + "]";
//            GLuint bonequaternion = glGetUniformLocation(body->programID, name_quad.c_str());
//            glUniform4fv(bonequaternion, 1, value_ptr(quaternions[i]));
            
            const string name_quad = "quaternions[" + to_string(i) + "]";
            GLuint bonequaternion = glGetUniformLocation(body->programID, name_quad.c_str());
            glUniform4fv(bonequaternion, 1,(const GLfloat*)(&quaternions[i]));
            
            const string name_tras = "traslations[" + to_string(i) + "]";
            GLuint bonetraslation = glGetUniformLocation(body->programID, name_tras.c_str());
            glUniformMatrix4fv(/*m_boneLocation[i]*/bonetraslation, 1, GL_TRUE, (const GLfloat*)traslations[i]);
//
//            const string name_scal = "scales[" + to_string(i) + "]";
//            GLuint bonescale = glGetUniformLocation(body->programID, name_scal.c_str());
//            glUniformMatrix4fv(/*m_boneLocation[i]*/bonescale, 1, GL_TRUE, (const GLfloat*)scales[i]);
            
//            const string name_pt = "pTransformations[" + to_string(i) + "]";
//            GLuint boneparentT = glGetUniformLocation(body->programID, name_pt.c_str());
//            glUniformMatrix4fv(/*m_boneLocation[i]*/boneparentT, 1, GL_TRUE, (const GLfloat*)pTransformations[i]);
            
            const string name_rt = "rotations[" + to_string(i) + "]";
            GLuint bonerotation = glGetUniformLocation(body->programID, name_rt.c_str());
            glUniformMatrix4fv(/*m_boneLocation[i]*/bonerotation, 1, GL_TRUE, (const GLfloat*)rotations[i]);
        }
        
//        for (unsigned int i = 0; i < Transforms.size(); ++i)
//        {
//            // std::cout << to_string(Transforms[i][0][0]) << std::endl;
//            // printGLMMat4(Transforms[i]);
//            const string name = "gBones[" + to_string(i) + "]";
//            GLuint boneTransform = glGetUniformLocation(body->programID, name.c_str());
//            glUniformMatrix4fv(boneTransform, 1, GL_TRUE, glm::value_ptr(Transforms[i]));
//        }

        //render
        gizmo->draw();

        //Dibujar en wireframe
        body->draw();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );

    // Cleanup VBO and shader
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

