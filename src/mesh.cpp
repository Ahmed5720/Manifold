#include "mesh.h"
#include <unordered_map>



// mutation helpers
// addFace, addVertex, addHalfEdge
int Mesh::addVertex(vec3 p)
{
    Vertex v;
    v.position = p;
    vertices.push_back(v);
    return vertices.size()-1; 
}
int Mesh::addface()
{
    faces.push_back(Face{});
    return faces.size()-1;
}
int Mesh::addHalfEdge()
{
    halfedges.push_back(halfEdge{});
    return halfedges.size()-1;
}
void Mesh::linkFaceLoop(int faceIdx, const vector<int>& heIndices)
{   
    int n = heIndices.size();
    for (int i = 0; i < n; i++)
    {
        halfEdge& he = halfedges[heIndices[i]];
        he.face = faceIdx;
        he.next = heIndices[(i + 1) % n];
        he.prev = heIndices[(i + n - 1) % n];
    }
    faces[faceIdx].halfedge = heIndices[0];
    
}

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
const int edgeKey(int vFrom, int vTo)
{
    return vFrom * 1000 + vTo; 
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


// computes vertex normals by taking average of face normals adjacent to all normals.
void Mesh::computeNormals()
{
    for (int vIdx = 0; vIdx < vertices.size(); vIdx++)
    {   
        vec3 N = {0.0f,0.0f,0.0f};
        forEachHalfEdgeOfVertex(vIdx, [&](int heIndex)
        {   
            int f = halfedges[heIndex].face;
            if(f != -1)
                N+= faceNormal(f);
        });
        vertices[vIdx].normal = normalize(N);
    }
    
}

// generators
// makeCube

/*
constructing a mesh is a little tricky, we have to define many things
but the time spent in construction becomes worth it it traversal .. as with many things in life arguably..
first we have to define our vertices, for a cube we have 8
then we have to define faces in ccw order such that we can identify what the normal of the face is based on the order of vertices
next we need to define our half edges, each face has its OWN 4 half edges, totalling 24 half edges. 4 * 6
we move along the vertices of each face, set that vertex to be the half edges vertex, set the next half edge, to be the next half edge and the prev to be the prev
next we stitch those faces together by identifying each half edge's twin. to do so, we build a map that maps each he index to a unique number based on its from and to vertex
we then walk across the entire mesh's hedges, and if a half edge doesnt have a twin, we get its vertex, and its next's vertex, use that to hash into our function to return the index of its twin and we set that as its twin

*/
//     7---------- 6     Y
//  4  :        5 :      |__ X
//  |  :        | :     /
//  |  3 -------|- 2   Z
//  | /         |/
//  0 --------- 1 

Mesh Mesh :: makeCube()
{
    Mesh m;
    m.addVertex({0,0,1}); // v0
    m.addVertex({1,0,1}); // v1
    m.addVertex({1,0,0}); // v2
    m.addVertex({0,0,0}); // v3
    m.addVertex({0,1,1}); // v4
    m.addVertex({1,1,1}); // v5
    m.addVertex({1,1,0}); // v6
    m.addVertex({0,1,0}); // v7

    const int faceVerts[6][4] = {
        {0, 1, 5, 4},  // front
        {1, 2, 6, 5},  // right
        {2, 3, 7, 6},  // back
        {3, 0, 4, 7},  // left
        {3, 2, 1, 0},  // bottom
        {4, 5, 6, 7},  // top
    };
    
    unordered_map <int, int> edgeMap;
    for (int fi = 0; fi < 6; fi++)
    {   
        const int faceIdx = m.addface();
        vector<int> heLoop;
        heLoop.reserve(4);
        for (int ei = 0; ei < 4; ei++)
        {
            const int vFrom = faceVerts[fi][ei];
            const int vTo   = faceVerts[fi][(ei+1) % 4];

            const int hIdx = m.addHalfEdge();
            m.halfedges[hIdx].vertex = vFrom;

            if (m.vertices[vFrom].halfedge == -1)
                m.vertices[vFrom].halfedge = hIdx;
            heLoop.push_back(hIdx);
            
            // saves the edge into the edge map so we can stitch twins later
            edgeMap[edgeKey(vFrom, vTo)] = hIdx;
        }

        m.linkFaceLoop(faceIdx, heLoop);
    }


    // finally stitching half edges by finding each twin in the edgemap
    for (int i = 0; i < m.halfedges.size(); i++)
    {   
        
        halfEdge& he = m.halfedges[i];
        if(he.twin != -1)
            continue;
        int vFrom = he.vertex;
        int vTo = m.halfedges[he.next].vertex;
        int key = edgeKey(vTo, vFrom);
        if(edgeMap.count(key))
        {   
            int twinIdx = edgeMap[key];
            he.twin = twinIdx;  
            m.halfedges[twinIdx].twin = i;
        }
            
    }

    m.computeNormals();
    return m;
}