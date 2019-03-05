#version 330 core

// Aqui van los vertex buffer que mandamos al GPU
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

// datos de salida hacia el fragment shader (lo que tenemos que calcular)
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 We;

const int MAX_BONES = 100;
// Datos uniformes al objeto
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;
uniform mat4 gBones[MAX_BONES];

void main(){

     mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
     BoneTransform += gBones[BoneIDs[1]] * Weights[1];
     BoneTransform += gBones[BoneIDs[2]] * Weights[2];
     BoneTransform += gBones[BoneIDs[3]] * Weights[3];
////
     vec4 PosL    = BoneTransform * vec4(vertexPosition_modelspace, 1.0);//4*1
     gl_Position  = MVP * PosL;//4*1
    
//    gl_Position = MVP* vec4(vertexPosition_modelspace,1);
    
    Normal_cameraspace = ( transpose(inverse(V * M)) * BoneTransform * vec4(vertexNormal_modelspace,0)).xyz;
//
//     vec4 worldNormal = gBones[BoneIDs[0]]* vec4(vertexNormal_modelspace, 0.0)*Weights[0];
//     worldNormal += gBones[BoneIDs[1]]* vec4(vertexNormal_modelspace, 0.0)*Weights[1];
//     worldNormal += gBones[BoneIDs[2]]* vec4(vertexNormal_modelspace, 0.0)*Weights[2];
//     worldNormal += gBones[BoneIDs[3]]* vec4(vertexNormal_modelspace, 0.0)*Weights[3];
//
//     Normal_cameraspace =  (transpose(inverse(V * M))*worldNormal).xyz;
    
//    vec4 totalLocalPos = vec4(0.0);
//    vec4 totalNormal = vec4(0.0);
//    for(int i=0;i<4;i++){
//        mat4 boneTransformation = gBones[BoneIDs[i]];
//
//        vec4 posePosition = boneTransformation* vec4(vertexPosition_modelspace,1.0);
//        totalLocalPos += posePosition*Weights[i];
//
//        vec4 worldNormal = boneTransformation*vec4(vertexNormal_modelspace,0.0);
//        totalNormal += worldNormal*Weights[i];
//    }
//
//     gl_Position = MVP*totalLocalPos;
////    gl_Position = MVP* vec4(vertexPosition_modelspace,1);
//    Normal_cameraspace = (transpose(inverse(V * M)) *totalNormal).xyz;



//
//    // gl_position es la position del vertice despues de la proyeccion
////    gl_Position = MVP * vec4(vertexPosition_modelspace,1);
//
//    // La posicion del vertice solamente despues de la transformacion espacial (rotacion)
    Position_worldspace = (M * vec4(vertexPosition_modelspace,1)).xyz;
//
//    // Vector que va del vertice hacia la camara
    // En el espacio camara la posicion de la camara es 0,0,0
    vec3 vertexPosition_cameraspace = ( V * M * vec4(vertexPosition_modelspace,1)).xyz;
    EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
//
//    // Vector que va del vertice hacia la camara( espacio camara) No hay M porque no transformamos la luz.
    vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace,1)).xyz;
    LightDirection_cameraspace = LightPosition_cameraspace - vertexPosition_cameraspace;
//
//    // Normal despues de la transformacion
    // Normal_cameraspace = ( transpose(inverse(V * M)) * vec4(vertexNormal_modelspace,0)).xyz;
//
//    // UV no hacemos nada mas que interpolacion
//    UV = vertexUV;
//
    UV    = vertexUV;
    We = Weights;
}

