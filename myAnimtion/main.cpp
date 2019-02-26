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
using namespace glm;


#include "object3d.h"
#include "gizmo.h"

#include "common/shader.hpp"
#include "common/texture.hpp"
#include "common/controls.hpp"
#include "common/objloader.hpp"
#include "common/vboindexer.hpp"
#include "constants.h"



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
    string path;
    
    //SINO con OBJ
//    Object3D* body = new Object3D(PATH+"data/batma.obj",true);
//     Object3D* body = new Object3D(PATH+"data/models/gilbert/source/gilbert.fbx",true);
//    Object3D* body = new Object3D(PATH+"data/models/big-guy/source/big_guy.fbx",true);
   Object3D* body = new Object3D(PATH+"data/models/big-guy.fbx",true);
    // Object3D* body = new Object3D(PATH+"data/models/Monster_1/Monster_1.dae",true);
    // Object3D* body = new Object3D("data/batma.obj",false);
//     Object3D* body = new Object3D(PATH+"data/models/ArmatureStraight.obj",true);
    


    body->setShaders(PATH+"StandardShading.vertexshader.glsl", PATH+"phong.fragmentshader.glsl");
    //Object3D* trex = new Object3D("data/trex/TrexByJoel3d.fbx");
    //trex->setShaders("StandardShading.vertexshader", "phong.fragmentshader");


    Gizmo* gizmo = new Gizmo();
    gizmo->setShaders(PATH+"gizmo.vertexshader", PATH+"gizmo.fragmentshader");


    //textuta

    GLuint Texture= loadDDS((PATH+"data/cube/uvmap.DDS").c_str());
    GLuint TextureID  = glGetUniformLocation(gizmo->programID, (PATH+"myTextureSampler").c_str());

    // GLuint textureID;
    // glGenTextures(1,&textureID);
    // glBindTexture(GL_TEXTURE_2D,textureID);

    // glTextureImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_BGR,GL_UNSIGNED_BYTE,data);



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

//#include <iostream>
//
//// GLEW
//#define GLEW_STATIC
//#include <GL/glew.h>
//
//// GLFW
//#include <GLFW/glfw3.h>
//
//// Window dimensions
//const GLuint WIDTH = 800, HEIGHT = 600;
//
//// Shaders
//const GLchar* vertexShaderSource = "#version 330 core\n"
//"layout (location = 0) in vec3 position;\n"
//"void main()\n"
//"{\n"
//"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
//"}\0";
//const GLchar* fragmentShaderSource = "#version 330 core\n"
//"out vec4 color;\n"
//"void main()\n"
//"{\n"
//"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//"}\n\0";
//
//// The MAIN function, from here we start the application and run the game loop
//int main()
//{
//    // Init GLFW
//    glfwInit( );
//
//    // Set all the required options for GLFW
//    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
//    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
//    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
//    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
//
//    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
//
//    // Create a GLFWwindow object that we can use for GLFW's functions
//    GLFWwindow *window = glfwCreateWindow( WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr );
//
//    int screenWidth, screenHeight;
//    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );
//
//    if ( nullptr == window )
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate( );
//
//        return EXIT_FAILURE;
//    }
//
//    glfwMakeContextCurrent( window );
//
//    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
//    glewExperimental = GL_TRUE;
//    // Initialize GLEW to setup the OpenGL Function pointers
//    if ( GLEW_OK != glewInit( ) )
//    {
//        std::cout << "Failed to initialize GLEW" << std::endl;
//        return EXIT_FAILURE;
//    }
//
//    // Define the viewport dimensions
//    glViewport( 0, 0, screenWidth, screenHeight );
//
//
//    // Build and compile our shader program
//    // Vertex shader
//    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
//    glShaderSource( vertexShader, 1, &vertexShaderSource, NULL );
//    glCompileShader( vertexShader );
//
//    // Check for compile time errors
//    GLint success;
//    GLchar infoLog[512];
//
//    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );
//    if ( !success )
//    {
//        glGetShaderInfoLog( vertexShader, 512, NULL, infoLog );
//        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
//
//    // Fragment shader
//    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
//    glShaderSource( fragmentShader, 1, &fragmentShaderSource, NULL );
//    glCompileShader( fragmentShader );
//
//    // Check for compile time errors
//    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );
//
//    if ( !success )
//    {
//        glGetShaderInfoLog( fragmentShader, 512, NULL, infoLog );
//        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
//    }
//
//    // Link shaders
//    GLuint shaderProgram = glCreateProgram( );
//    glAttachShader( shaderProgram, vertexShader );
//    glAttachShader( shaderProgram, fragmentShader );
//    glLinkProgram( shaderProgram );
//
//    // Check for linking errors
//    glGetProgramiv( shaderProgram, GL_LINK_STATUS, &success );
//
//    if ( !success )
//    {
//        glGetProgramInfoLog( shaderProgram, 512, NULL, infoLog );
//        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
//    }
//
//    glDeleteShader( vertexShader );
//    glDeleteShader( fragmentShader );
//
//
//    // Set up vertex data (and buffer(s)) and attribute pointers
//    GLfloat vertices[] =
//    {
//        -0.5f, -0.5f, 0.0f, // Left
//        0.5f, -0.5f, 0.0f, // Right
//        0.0f,  0.5f, 0.0f  // Top
//    };
//
//    GLuint VBO, VAO;
//    glGenVertexArrays( 1, &VAO );
//    glGenBuffers( 1, &VBO );
//    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
//    glBindVertexArray( VAO );
//
//    glBindBuffer( GL_ARRAY_BUFFER, VBO );
//    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
//
//    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof( GLfloat ), ( GLvoid * ) 0 );
//    glEnableVertexAttribArray( 0 );
//
//    glBindBuffer( GL_ARRAY_BUFFER, 0 ); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
//
//    glBindVertexArray( 0 ); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
//
//    // Game loop
//    while ( !glfwWindowShouldClose( window ) )
//    {
//        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
//        glfwPollEvents( );
//
//        // Render
//        // Clear the colorbuffer
//        glClearColor( 0.2f, 0.3f, 0.3f, 1.0f );
//        glClear( GL_COLOR_BUFFER_BIT );
//
//        // Draw our first triangle
//        glUseProgram( shaderProgram );
//        glBindVertexArray( VAO );
//        glDrawArrays( GL_TRIANGLES, 0, 3 );
//        glBindVertexArray( 0 );
//
//        // Swap the screen buffers
//        glfwSwapBuffers( window );
//    }
//
//    // Properly de-allocate all resources once they've outlived their purpose
//    glDeleteVertexArrays( 1, &VAO );
//    glDeleteBuffers( 1, &VBO );
//
//    // Terminate GLFW, clearing any resources allocated by GLFW.
//    glfwTerminate( );
//
//    return EXIT_SUCCESS;
//}
