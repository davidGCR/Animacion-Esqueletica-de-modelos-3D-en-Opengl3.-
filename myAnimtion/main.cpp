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
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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
     Object3D* body = new Object3D(PATH+"data/models/model.dae",true);
    // Object3D* body = new Object3D(PATH+"data/models/Monster_1/Monster_1.dae",true);
    // Object3D* body = new Object3D("data/batma.obj",false);
    
//     Object3D* body = new Object3D(PATH+"data/models/boblampclean.md5anim",true);
//Object3D* trex = new Object3D("data/trex/TrexByJoel3d.fbx");
    //trex->setShaders("StandardShading.vertexshader", "phong.fragmentshader");

    body->setShaders(PATH+"StandardShading.vertexshader.glsl", PATH+"phong.fragmentshader.glsl");

    Gizmo* gizmo = new Gizmo();
    gizmo->setShaders(PATH+"gizmo.vertexshader", PATH+"gizmo.fragmentshader");

    // // //GEt uniform location: array
//     GLuint m_boneLocation[MAX_BONES];
//     int tam = sizeof(m_boneLocation)/sizeof(m_boneLocation[0]);
//
//     for (unsigned int i = 0 ; i < tam ; i++) {
//         char Name[128];
//         memset(Name, 0, sizeof(Name));
//         snprintf(Name, sizeof(Name), "gBones[%d]", i);
//         m_boneLocation[i] = GetUniformLocation(body->programID,Name);
//     }


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
    do{
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

        cout<<"time: "<<t<<endl;
        vector<glm::mat4> Transforms;
        body->BoneTransform(t,Transforms);

        // cout<<"IDENTITY"<<endl;
        // glm::mat4 Identity(0);
        // printGLMMat4(Identity);
        
        for (unsigned int i = 0; i < Transforms.size(); ++i)
        {
            // std::cout << to_string(Transforms[i][0][0]) << std::endl;
            // printGLMMat4(Transforms[i]);
            const std::string name = "gBones[" + std::to_string(i) + "]";
            GLuint boneTransform = glGetUniformLocation(body->programID, name.c_str());
            glUniformMatrix4fv(boneTransform, 1, GL_FALSE, glm::value_ptr(Transforms[i]));
        }

        //render
        gizmo->draw();

        //Dibujar en wireframe
        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
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

