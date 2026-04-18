#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <functional>

using namespace glm;
using namespace std;

struct halfEdge
{
    int next = -1;
    int prev = -1;
    int twin = -1;
    int vertex = -1;
    int face  = -1;
};

struct Vertex
{
    vec3 position = {0,0,0};
    vec3 normal = {0,0,0};
    int halfedge = -1; // stores any outgoing half edge from vertex
};

struct Face
{
    bool selected = false;
    int halfedge = -1; // stores any half edge belonging to the face.
};


const int edgeKey(int vFrom, int vTo);
class Mesh
{
    public:
        vector<halfEdge> halfedges;
        vector<Vertex> vertices;
        vector<Face> faces;

        static Mesh makeCube();

        // traversal methods

        void forEachHalfEdgeofFace(int startHe, const function<void(int)>& cb) const;
        void forEachHalfEdgeOfVertex(int vert, const function<void(int)>& cb) const;
        
        // returns list of all vertices of a face
        vector<int> faceVertices(int faceIdx) const;

        // computes normals of all vertices as an area weighted average of face normals
        void computeNormals();

        // gets centroid of a face as average of vertex positions
        vec3 faceCentroid(int faceIdx) const;
        // cross product between two edges of a face, normalized
        vec3 faceNormal(int faceIdx) const;

        // mutations

        int addVertex(vec3 pos);
        int addface();
        int addHalfEdge();

        void linkFaceLoop(int faceIdx, const vector<int>& heIndices);
        int findHalfEdge(int fromVertex, int toVertex) const;


};