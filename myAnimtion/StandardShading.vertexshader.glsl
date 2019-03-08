#version 330 core

// Aqui van los vertex buffer que mandamos al GPU
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;
layout(location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;
layout (location = 5) in vec3 cor;

// datos de salida hacia el fragment shader (lo que tenemos que calcular)
out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 We;
//out vec3 fcors;

const int MAX_BONES = 70;
// Datos uniformes al objeto
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;
uniform mat4 gBones[MAX_BONES];
//uniform vec3 traslations_rest[MAX_BONES];
//uniform vec3 traslations_pose[MAX_BONES];
uniform mat4 rotations[MAX_BONES];
uniform vec4 quaternions[MAX_BONES];
uniform mat4 traslations[MAX_BONES];

//uniform mat4 globalTransformation;
//uniform mat4 offs[MAX_BONES];
//uniform mat4 scales[MAX_BONES];
//uniform mat4 pTransformations[MAX_BONES];

vec4 AntipodalityAwareAdd(vec4 q1, vec4 q2);
mat4 toRotationMatrix(vec4 quat);

void main(){


     mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
     BoneTransform += gBones[BoneIDs[1]] * Weights[1];
     BoneTransform += gBones[BoneIDs[2]] * Weights[2];
     BoneTransform += gBones[BoneIDs[3]] * Weights[3];

     vec4 PosL    = BoneTransform * vec4(vertexPosition_modelspace, 1.0);//4*1
//     gl_Position  = MVP * PosL;//4*1 //pos en LBS
//    gl_Position = MVP*vec4(vertexPosition_modelspace,1);
//
    /***************************************************/
    
    
    mat4 t_rotation;
    mat4 t_traslation;
    int idx=0;
    for (int i=0; i<4; i++) {
        idx = BoneIDs[i];
//        tt += pTransformations[idx]* traslations[idx] * rotations[idx] * scales[idx]  * Weights[i];
        //tt +=  (traslations[idx]) * (rotations[idx]) * scales[idx] * Weights[i];
        t_rotation += rotations[idx]*Weights[i];
        t_traslation += traslations[idx];
    }
//
    vec4 tras;
    tras.x = t_traslation[3][0];
    tras.y = t_traslation[3][1];
    tras.z = t_traslation[3][2];
    tras.w = 1;
//    vec4 pos = t_rotation*vec4(vertexPosition_modelspace,1.0) + tras;

    /*............:::::::::::::::::::: QLERP .........::::::::::::::*/
    vec4 Q0 = quaternions[BoneIDs[0]]*Weights[0];
    vec4 Q1 = quaternions[BoneIDs[1]]*Weights[1];
    vec4 Q2 = quaternions[BoneIDs[2]]*Weights[2];
    vec4 Q3 = quaternions[BoneIDs[3]]*Weights[3];

    vec4 q0q1 = AntipodalityAwareAdd(Q0,Q1);
    vec4 q2q3 = AntipodalityAwareAdd(Q2,Q3);
    vec4 Q = AntipodalityAwareAdd(q0q1,q2q3);
    Q = normalize(Q);
    mat4 R = toRotationMatrix(Q);
//    vec4 T = t_rotation * vec4(cor,1.0) + tras - R * vec4(cor,1.0);
     vec4 T = t_rotation * vec4(cor,1.0) + tras - t_rotation * vec4(cor,1.0);
    float total_peso = Weights[0]+Weights[1]+Weights[2]+Weights[3];
   
//    if(dot(cor,cor) > 0.0){
////       gl_Position = MVP  * (t_rotation*vec4(vertexPosition_modelspace,1.0) + T);
////        gl_Position = MVP  * (t_rotation*vec4(cor,1.0)  + T);
//        We = vec4(1.0,1.0,1.0,1.0);
//    }else{
////        gl_Position = MVP  * pos;
//
//         We = Weights;
//    }
    gl_Position = MVP  * (R*vec4(vertexPosition_modelspace,1.0) + T);
    

    Normal_cameraspace = ( transpose(inverse(V * M)) * BoneTransform * vec4(vertexNormal_modelspace,0)).xyz;
//    // gl_position es la position del vertice despues de la proyeccion
//    gl_Position = MVP * vec4(vertexPosition_modelspace,1);
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
    We = Weights;
    UV    = vertexUV;
    
    //fcors = cors;
}

vec4 AntipodalityAwareAdd(vec4 q1, vec4 q2)
{
    if (dot(q1,q2) >= 0) {
        return q1 + q2;
    } else {
        return q1 + (-q2);
    }
    //return vec4(0.0,0.0,0.0,0.0);
}
mat4 toRotationMatrix(vec4 quat) {
    mat4 matrix;
    //    float xy = quad.x * quad.y;
    //     float xz = quad.x * quad.z;
    //     float xw = quad.x * quad.w;
    //     float yz = quad.y * quad.z;
    //     float yw = quad.y * quad.w;
    //     float zw = quad.z * quad.w;
    //     float xSquared = quad.x * quad.x;
    //     float ySquared = quad.y * quad.y;
    //     float zSquared = quad.z * quad.z;
    //    matrix[0][0] = 1 - 2 * (ySquared + zSquared);
    //    matrix[1][0] = 2 * (xy - zw);
    //    matrix[2][0] = 2 * (xz + yw);
    //    matrix[3][0]= 0;
    //    matrix[0][1] = 2 * (xy + zw);
    //    matrix[1][1] = 1 - 2 * (xSquared + zSquared);
    //    matrix[2][1] = 2 * (yz - xw);
    //    matrix[3][1] = 0;
    //    matrix[0][2] = 2 * (xz - yw);
    //    matrix[1][2] = 2 * (yz + xw);
    //    matrix[2][2] = 1 - 2 * (xSquared + ySquared);
    //    matrix[3][2] = 0;
    //    matrix[0][3] = 0;
    //    matrix[1][3] = 0;
    //    matrix[2][3] = 0;
    //    matrix[3][3] = 1;
    
    //    matrix[0][0] = 1 - 2 * (ySquared + zSquared);
    //    matrix[0][1] = 2 * (xy - zw);
    //    matrix[0][2] = 2 * (xz + yw);
    //    matrix[0][3]= 0;
    //    matrix[1][0] = 2 * (xy + zw);
    //    matrix[1][1] = 1 - 2 * (xSquared + zSquared);
    //    matrix[1][2] = 2 * (yz - xw);
    //    matrix[1][3] = 0;
    //    matrix[2][0] = 2 * (xz - yw);
    //    matrix[2][1] = 2 * (yz + xw);
    //    matrix[2][2] = 1 - 2 * (xSquared + ySquared);
    //    matrix[2][3] = 0;
    //    matrix[3][0] = 0;
    //    matrix[3][1] = 0;
    //    matrix[3][2] = 0;
    //    matrix[3][3] = 1;
    float yy2 = 2.0f * quat.y * quat.y;
    float xy2 = 2.0f * quat.x * quat.y;
    float xz2 = 2.0f * quat.x * quat.z;
    float yz2 = 2.0f * quat.y * quat.z;
    float zz2 = 2.0f * quat.z * quat.z;
    float wz2 = 2.0f * quat.w * quat.z;
    float wy2 = 2.0f * quat.w * quat.y;
    float wx2 = 2.0f * quat.w * quat.x;
    float xx2 = 2.0f * quat.x * quat.x;
    matrix[0][0] = - yy2 - zz2 + 1.0f;
    matrix[0][1] = xy2 + wz2;
    matrix[0][2] = xz2 - wy2;
    matrix[0][3] = 0;
    matrix[1][0] = xy2 - wz2;
    matrix[1][1] = - xx2 - zz2 + 1.0f;
    matrix[1][2] = yz2 + wx2;
    matrix[1][3] = 0;
    matrix[2][0] = xz2 + wy2;
    matrix[2][1] = yz2 - wx2;
    matrix[2][2] = - xx2 - yy2 + 1.0f;
    matrix[2][3] = 0.0f;
    matrix[3][0] = matrix[3][1] = matrix[3][2] = 0;
    matrix[3][3] = 1.0f;
    return matrix;
}
vec3 rotar(vec4 Q, vec3 vertex){
    //make rotation with quaternion
    vec3 temp = cross(Q.xyz, vertex) + Q.w * vertex;
    vec3 rotated2 = vertex + 2.0*cross(Q.xyz, temp);
    return rotated2;
}

