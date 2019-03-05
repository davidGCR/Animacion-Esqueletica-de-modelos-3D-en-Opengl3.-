
#include <iostream>
#include <vector>
#include <cstring>
#include <assert.h>
#include <glm/glm.hpp>
#include "sk_class/my_math.h"
#include "object3d.h"

using namespace std;

struct TriangleIndices
{
    int alpha, beta, gamma;
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

// typedef vector<float> BoneWeightMap;

// VertexBoneData -> sizze: total vertices
// BonesInfo -> size: total bones
void SubdivideMesh(const vector<Vector3f> &verticesIn, const vector<TriangleIndices> &trianglesIn, const vector<VertexBoneData> &boneWeightsPerVertexIn, float epsilon);
float ComputeL2SkinningWeightDistance(const VertexBoneData &wP, const VertexBoneData &wV);
void SubdivideEdge(vector<Vector3f> &verticesInOut, vector<TriangleIndices> &trianglesInOut, vector<VertexBoneData> &boneWeightsInOut, int triangleIdx, int edgeStart);
vector<TriangleCentroidData> PrecalculateTriangleData(const vector<Vector3f> &vertices, const vector<TriangleIndices> &triangles, const vector<VertexBoneData> &weightsPerVertex);
float ComputeTriangleArea(const TriangleNodes &triangle);
VertexBoneData ComputeTriangleAverageWeights(const vector<VertexBoneData> &boneWeights, const TriangleIndices &triangle);
float ComputeSimilarity(const VertexBoneData &wP, const VertexBoneData &wV, float sigma);

bool ComputeOptimizedCoR(int vertexIndex, const vector<Vector3f> &vertices, const vector<VertexBoneData> &boneWeightsPerVertex, const vector<TriangleCentroidData> &triangleDataPerTriangle, float sigma, Vector3f &corOut);

void ComputeOptimizedCoRs(vector<Vector3f> &Positions, vector<unsigned int> &Indices, vector<BoneInfo> bonesInfo, vector<VertexBoneData> bones)
{
    vector<TriangleIndices> triangles(Indices.size() / 3);
    for (int triangleIndex = 0; triangleIndex < Indices.size() / 3; triangleIndex++)
    {
        triangles[triangleIndex].alpha = Indices[triangleIndex * 3 + 0];
        triangles[triangleIndex].beta = Indices[triangleIndex * 3 + 1];
        triangles[triangleIndex].gamma = Indices[triangleIndex * 3 + 2];
    }

    
    // vector<BoneWeightMap> boneWeights;
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
}

void SubdivideMesh(const vector<Vector3f> &verticesIn, const vector<TriangleIndices> &trianglesIn, const vector<VertexBoneData> &boneWeightsPerVertexIn, float epsilon)
{
    vector<Vector3f> vertices = verticesIn;
    vector<TriangleIndices> triangles = trianglesIn;
    vector<VertexBoneData> boneWeightsPerVertex = boneWeightsPerVertexIn;

    for (int triangleIdx = 0; triangleIdx < triangles.size(); triangleIdx++)
    {
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
    
    //return {vertices, triangles, boneWeightsPerVertex};
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

void SubdivideEdge(vector<Vector3f> &verticesInOut, vector<TriangleIndices> &trianglesInOut, vector<VertexBoneData> &boneWeightsInOut, int triangleIdx, int edgeStart)
{

    TriangleIndices triangle = trianglesInOut[triangleIdx];

    int alpha = triangle[edgeStart];
    int beta = triangle[(edgeStart + 1) % 3];
    int gamma = triangle[(edgeStart + 2) % 3];

    Vector3f newV = (verticesInOut[alpha] + verticesInOut[beta]) * 0.5f;
    int newVIdx = int(verticesInOut.size());
    verticesInOut.push_back(newV);

    VertexBoneData newBoneWeights;
    for (int i = 0; i < 4; i++)
    {
        newBoneWeights.Weights[i] = (boneWeightsInOut[alpha].Weights[i] + boneWeightsInOut[beta].Weights[i]) * 0.5f;
    }

    // newBoneWeights = (boneWeightsInOut[alpha] + boneWeightsInOut[beta]) * 0.5f;
    boneWeightsInOut.push_back(newBoneWeights);

    trianglesInOut.push_back({alpha, newVIdx, gamma});
    trianglesInOut.push_back({newVIdx, beta, gamma});
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
            auto wPj = wP.Weights[j];
            auto wPk = wP.Weights[k];
            auto wVj = wV.Weights[j];
            auto wVk = wV.Weights[k];

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

    for (const TriangleCentroidData &t : triangleDataPerTriangle)
    {

        const VertexBoneData &wI = boneWeightsPerVertex.at((size_t)vertexIndex);
        const VertexBoneData &wTriangleAverage = t.averageBoneWeights;

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
