// #ifndef UTILSS_H
// #define UTILSS_H

#include <sys/time.h>
#include <iostream>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

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

// print out matrix by rows
void util_printGLMMat4(glm::mat4  mat){
  int i,j;
  for (j=0; j<4; j++){
    for (i=0; i<4; i++){
    printf("%f ",mat[i][j]);
  }
  printf("\n");
 }
}



// #endif