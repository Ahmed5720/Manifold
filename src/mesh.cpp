#include "mesh.h"
#include <unordered_map>



// mutation helpers
// addFace, addVertex, addHalfEdge



// traversal 

// since face stores in it a half edge, we simply start from that half edge and keep walking along each of its next half edges until we get back to where we started from
void Mesh::forEachHalfEdgeofFace(int startHe, const function<void(int)>& cb) const
{
    int cur = startHe;
    cb(cur);
    cur = halfedges[cur].next;
    while(cur != startHe)
    {
        cb(cur);
        cur = halfedges[cur].next;
    }
}

// for vertex traversal, its a little less trivial, we start at a half edge in the same way, but then to get to the next adjacent half edge, we need to revert our direction then pick the next ccw half edge
// so we get half edge -> apply the function to it -> get its twin -> then get next half edge -> apply function to it etc.. 

// potentiallly buggy
void Mesh::forEachHalfEdgeOfVertex(int vert, const function<void(int)>& cb) const
{
    int startHe = vertices[vert].halfedge;
    int cur = startHe;
    if(cur == -1)
        return;
    cb(cur);
    cur = halfedges[cur].twin;
    cur = halfedges[cur].next;
    while(cur != startHe)
    {
        cb(cur);
        cur = halfedges[cur].twin;
        if(cur == -1)
            break;
        cur = halfedges[cur].next;
    }
}

//returns a vector of vertices belonging to a face
// gets haveedges from face, then get vertex from them
vector<int> Mesh::faceVertices(int faceIdx) const
{
    vector<int> vertices;
    int startHe = faces[faceIdx].halfedge;
    forEachHalfEdgeofFace(startHe, [&](int heIndex)
    {
        vertices.push_back(halfedges[heIndex].vertex);        
    });
    return vertices;
}

// geometry helpers
//faceCentroid, faceNormal, computeNormals
vec3 Mesh::faceCentroid(int faceIdx) const
{   
    vec3 centroid;
    int cnt = 0;
    int startHe = faces[faceIdx].halfedge;
    forEachHalfEdgeofFace(startHe, [&](int heIndex)
    {
        centroid += vertices[halfedges[heIndex].vertex].position;
        cnt ++;
    });

    return cnt > 0 ? centroid / static_cast<float>(cnt) : centroid;

}

// face normal is just cross any two face half edges
vec3 Mesh::faceNormal(int faceIdx) const
{
    halfEdge he = halfedges[faces[faceIdx].halfedge];
    halfEdge he_next = halfedges[he.next];
    halfEdge he_next_next = halfedges[he_next.next];
    vec3 v1 = vertices[he.vertex].position;
    vec3 v2 = vertices[he_next.vertex].position;
    vec3 v3 = vertices[he_next_next.vertex].position;
    vec3 e1 = v2 - v1;
    vec3 e2 = v3 - v1;
    return normalize(cross(e1, e2));
}

void Mesh::computeNormals()
{

}

// generators
// makeCube

/*
constructing a mesh is a little tricky, we have to define many things
but the time spent in construction becomes worth it it traversal .. as with many things in life arguably..
first we have to define our vertices, for a cube we have 8
then we have to define faces in ccw order such that we can identify what the normal of the face is based on the order of vertices
next we need to define our half edges, each face has 4 half edges we can construct them from vertices
*/ 

Mesh Mesh :: makeCube()
{
    Mesh m;

    return m;
}