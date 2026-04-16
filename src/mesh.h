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




};