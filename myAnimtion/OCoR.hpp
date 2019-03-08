//
//  ocor.hpp
//  myAnimtion
//
//  Created by David Choqueluque Roman on 3/5/19.
//  Copyright © 2019 David Choqueluque Roman. All rights reserved.
//

#ifndef OCOR_H
#define OCOR_H

#include <iostream>
#include <vector>
#include <cstring>
#include <assert.h>


// #include <glm/glm.hpp>
#include "sk_class/my_math.h"
#include "object3d.h"

using namespace std;

struct TriangleIndices
{
    int alpha, beta, gamma;
    TriangleIndices(){
        alpha = 0;
        beta = 0;
        gamma = 0;
    }
    TriangleIndices(int alp, int bet, int gam){
        alpha = alp;
        beta = bet;
        gamma = gam;
    }
    int operator[](int which) const
    {
        switch (which)
        {
            case 0:
            return alpha;
            case 1:
            return beta;
            case 2:
            return gamma;
        }
        assert(false);
        return 0;
    }
};

struct TriangleCentroidData
{
    Vector3f centroid;
    VertexBoneData averageBoneWeights;
    float area;
};
struct TriangleNodes
{
    Vector3f alpha, beta, gamma;
};

struct SubdividedMesh
{
    vector<Vector3f> vertices; // the first n vertices are identical to the source mesh, where n = source mesh vertex count
    vector<TriangleIndices> triangles;
    vector<VertexBoneData> boneWeightsPerVertex;
};

// typedef vector<float> BoneWeightMap;

// VertexBoneData -> sizze: total vertices
// BonesInfo -> size: total bones
SubdividedMesh SubdivideMesh(const vector<Vector3f> &verticesIn, const vector<TriangleIndices> &trianglesIn, const vector<VertexBoneData> &boneWeightsPerVertexIn, float epsilon);

float ComputeL2SkinningWeightDistance(const VertexBoneData &wP, const VertexBoneData &wV);

void SubdivideEdge(vector<Vector3f> &verticesInOut, vector<TriangleIndices> &trianglesInOut, vector<VertexBoneData> &boneWeightsInOut, int triangleIdx, int edgeStart);

vector<TriangleCentroidData> PrecalculateTriangleData(const vector<Vector3f> &vertices, const vector<TriangleIndices> &triangles, const vector<VertexBoneData> &weightsPerVertex);

float ComputeTriangleArea(const TriangleNodes &triangle);

VertexBoneData ComputeTriangleAverageWeights(const vector<VertexBoneData> &boneWeights, const TriangleIndices &triangle);

float ComputeSimilarity(const VertexBoneData &wP, const VertexBoneData &wV, float sigma);

bool ComputeOptimizedCoR(int vertexIndex, const vector<Vector3f> &vertices, const vector<VertexBoneData> &boneWeightsPerVertex,
                         const vector<TriangleCentroidData> &triangleDataPerTriangle, float sigma, Vector3f &corOut);



map<int, Vector3f> ComputeOptimizedCoRs(vector<Vector3f> &Positions, vector<unsigned int> &Indices, vector<BoneInfo> bonesInfo, vector<VertexBoneData> bones, vector<Vector3f>& cors,float subdivisionEpsilon=0.1f, float similaritySigma=0.1f)
{
    vector<TriangleIndices> triangles(Indices.size());
    for (int triangleIndex = 0; triangleIndex < Indices.size() / 3; triangleIndex++)
    {
        triangles[triangleIndex].alpha = Indices[triangleIndex * 3 + 0];
        triangles[triangleIndex].beta = Indices[triangleIndex * 3 + 1];
        triangles[triangleIndex].gamma = Indices[triangleIndex * 3 + 2];
    }
    std::cout << "==> CoR: Num Triangles = " << triangles.size() << std::endl;
    
    SubdividedMesh mesh_s = SubdivideMesh(Positions, triangles, bones, subdivisionEpsilon);
    
    cout << "==> CoR: SubdividedMesh vertices = " << mesh_s.vertices.size() << std::endl;
    cout << "==> CoR: SubdividedMesh triangles = " << mesh_s.triangles.size() << std::endl;
    cout << "==> CoR: SubdividedMesh boneWeightsPerVertex = " << mesh_s.boneWeightsPerVertex.size() << std::endl;
    
    vector<TriangleCentroidData> triangleData = PrecalculateTriangleData(mesh_s.vertices, mesh_s.triangles, mesh_s.boneWeightsPerVertex);
    
    map<int, Vector3f> results;
    //vector<Vector3f> cors;
    unsigned int numUnsubdividedVertices = Positions.size();
//
    for (int i=0; i<numUnsubdividedVertices; i++) {
        Vector3f cor;
        bool hasCoR = ComputeOptimizedCoR(i, mesh_s.vertices, mesh_s.boneWeightsPerVertex, triangleData, similaritySigma, cor);
        if (hasCoR) {
            results[i] = cor;
            cors[i] = cor;
        }
    }

//    std::cout << "==> CoR: results = " << cors.size() << std::endl;
//    // vector<BoneWeightMap> boneWeights;
    // //vector<vector<>(total_bones)> boneWeights
    // boneWeights.assign(Positions.size(), BoneWeightMap(bonesInfo.size())); //size: total vertices
    // int boneIndex = 0;
    // for (int i = 0; i < bonesInfo.size(); i++)
    // {
    
    //     for (int j = 0; j < bones.size(); j++)
    //     {
    //         boneWeights.at(i).at();
    //     }
    // }
    //return cors;
    
    return results;
}

SubdividedMesh SubdivideMesh(const vector<Vector3f> &verticesIn, const vector<TriangleIndices> &trianglesIn, const vector<VertexBoneData> &boneWeightsPerVertexIn, float epsilon)
{
    vector<Vector3f> vertices = verticesIn;
    vector<TriangleIndices> triangles = trianglesIn;
    vector<VertexBoneData> boneWeightsPerVertex = boneWeightsPerVertexIn;
    
    for (int triangleIdx = 0; triangleIdx < triangles.size(); triangleIdx++)
    {
//        cout << "==> SubdivideMesh:  Triangle = " << triangleIdx << endl;
        TriangleIndices triangle = triangles[triangleIdx];
        for (int i = 0; i < 3; i++)
        {
            const VertexBoneData &w0 = boneWeightsPerVertex[triangle[i]];
            const VertexBoneData &w1 = boneWeightsPerVertex[triangle[(i + 1) % 3]];
            float l2Distance = ComputeL2SkinningWeightDistance(w0, w1);
            if (l2Distance > epsilon)
            {
                SubdivideEdge(vertices, triangles, boneWeightsPerVertex, triangleIdx, i);
                triangles.erase(triangles.begin() + triangleIdx);
                --triangleIdx;
                break;
            }
        }
    }
    SubdividedMesh result;
    result.vertices = vertices;
    result.triangles = triangles;
    result.boneWeightsPerVertex = boneWeightsPerVertex;
    
    return result;
}

float ComputeL2SkinningWeightDistance(const VertexBoneData &wP, const VertexBoneData &wV)
{
    float distanceSq = 0;
    for (int i = 0; i < 4; i++)
    {
        distanceSq += std::pow(wP.Weights[i] - wV.Weights[i], 2);
    }
    return std::sqrt(distanceSq);
}

void SubdivideEdge(vector<Vector3f> &verticesInOut, vector<TriangleIndices>& trianglesInOut, vector<VertexBoneData> &boneWeightsInOut, int triangleIdx, int edgeStart)
{
    
    TriangleIndices triangle = trianglesInOut[triangleIdx];
    
    int alpha = triangle[edgeStart];
    int beta = triangle[(edgeStart + 1) % 3];
    int gamma = triangle[(edgeStart + 2) % 3];
    
    Vector3f newV = (verticesInOut[alpha] + verticesInOut[beta]) * 0.5f;
    int newVIdx = int(verticesInOut.size());
    verticesInOut.push_back(newV);
//
    VertexBoneData newBoneWeights;
    for (int i = 0; i < 4; i++)
    {
        newBoneWeights.Weights[i] = (boneWeightsInOut[alpha].Weights[i] + boneWeightsInOut[beta].Weights[i]) * 0.5f;
    }
//
//    // newBoneWeights = (boneWeightsInOut[alpha] + boneWeightsInOut[beta]) * 0.5f;
    boneWeightsInOut.push_back(newBoneWeights);
//
    TriangleIndices t1;
    t1.alpha = alpha;
    t1.beta = newVIdx;
    t1.gamma = gamma;
    trianglesInOut.push_back(t1);
    
    TriangleIndices t2;
    t2.alpha = alpha;
    t2.beta = newVIdx;
    t2.gamma = gamma;
    trianglesInOut.push_back(t2);
}

vector<TriangleCentroidData> PrecalculateTriangleData(const vector<Vector3f> &vertices, const vector<TriangleIndices> &triangles, const vector<VertexBoneData> &weightsPerVertex)
{
    vector<TriangleCentroidData> result;
    result.reserve(triangles.size());
    
    for (const TriangleIndices &t : triangles)
    {
        TriangleNodes tNodes{vertices[t.alpha], vertices[t.beta], vertices[t.gamma]};
        TriangleCentroidData data;
        data.area = ComputeTriangleArea(tNodes);
        data.averageBoneWeights = ComputeTriangleAverageWeights(weightsPerVertex, t);
        data.centroid = (tNodes.alpha + tNodes.beta + tNodes.gamma) *(1/ 3.0f);
        result.push_back(data);
    }
    
    return result;
}

float ComputeTriangleArea(const TriangleNodes &triangle)
{
    Vector3f u = triangle.beta - triangle.alpha;
    Vector3f v = triangle.gamma - triangle.alpha;
    
    Vector3f c = u.Cross(v);
    return 0.5f * sqrt(c.x * c.x + c.y * c.y + c.z * c.z);
}

VertexBoneData ComputeTriangleAverageWeights(const vector<VertexBoneData> &boneWeights, const TriangleIndices &triangle)
{
    VertexBoneData result;
    for (const int vertexIndex : {triangle.alpha, triangle.beta, triangle.gamma})
    {
        for (int j = 0; j < 4; j++)
        {
            result.Weights[j] += boneWeights[vertexIndex].Weights[j];
        }
    }
    
    for (float &w : result.Weights)
    {
        w /= 3.0f;
    }
    return result;
}

// sigma = width of exponential kernel in eq.1
float ComputeSimilarity(const VertexBoneData &wP, const VertexBoneData &wV, float sigma)
{
    float similarity = 0;
    const float sigmaSq = sigma * sigma;
    
    for (int j = 0; j < 4; j++)
    {
        for (int k = j + 1; k < 4; k++)
        {
            float wPj = wP.Weights[j];
//            cout<<"Aquiiiiii;;; "<<wPj<<endl;
            float wPk = wP.Weights[k];
            float wVj = wV.Weights[j];
            float wVk = wV.Weights[k];
//
            similarity += wPj * wPk * wVj * wVk * std::exp(-(std::pow(wPj * wVk - wPk * wVj, 2)) / sigmaSq);
        }
    }
    
    return similarity;
}
bool ComputeOptimizedCoR(int vertexIndex, const vector<Vector3f> &vertices, const vector<VertexBoneData> &boneWeightsPerVertex,
                         const vector<TriangleCentroidData> &triangleDataPerTriangle, float sigma, Vector3f &corOut)
{
    float totalWeight = 0;
    Vector3f posAccumulator;
    
    for (int i=0;i< triangleDataPerTriangle.size();i++)
    {
        TriangleCentroidData  t = triangleDataPerTriangle[i];
        const VertexBoneData wI = boneWeightsPerVertex[vertexIndex];
        const VertexBoneData wTriangleAverage = t.averageBoneWeights;
        
        const float similarity = ComputeSimilarity(wI, wTriangleAverage, sigma);
        float weight = similarity * t.area;
        totalWeight += weight;

        Vector3f pos = t.centroid;
        posAccumulator += pos * weight;
    }
    
    if (totalWeight > 0)
    {
        corOut = posAccumulator*(1/totalWeight);
        return true;
    }
    return false;
}

#endif
