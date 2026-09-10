#include "extrude.h"

// later extruder should specify the mesh we are extruding, however currently
// we simply have a scene and the scene, contains a single mesh, so we just pass
// that around 
/*

the other thing that extrusion needs to do is to deselect the old face and instead select the new face

naming it turns out can bepretty hard, lets denote the vertices of the face we extrude
from as uloi = upper left old index 
*/
int Extruder::extrude(Scene& scene, int faceIdx)
{
    Mesh& m = scene.m_mesh;
    Face& f = m.faces[faceIdx];
    vec3 fnorm = {0,0,1}; // for now 
    vector<int> verts = m.faceVertices(faceIdx); 

    vec3 uln = m.vertices[verts[0]].position + offset * fnorm; // upper left new vert
    vec3 dln = m.vertices[verts[1]].position + offset * fnorm;
    vec3 drn = m.vertices[verts[2]].position + offset * fnorm;
    vec3 urn = m.vertices[verts[3]].position + offset * fnorm;

    // we need to reset face's half edges to null so that they stop being paired
    // so the new side faces can pair their half edges with the old edges
    // i wonder if that would work
    m.vertices[verts[0]].halfedge = -1;
    m.vertices[verts[1]].halfedge = -1;
    m.vertices[verts[2]].halfedge = -1;
    m.vertices[verts[3]].halfedge = -1;
    f.halfedge = -1;

    int uloi = verts[0]; // upper left old index: u l o i
    int dloi = verts[1];
    int droi = verts[2];
    int uroi = verts[3];
    
    int ulni = m.addVertex(uln); // upper left new index: u l n i  
    int dlni = m.addVertex(dln);
    int drni = m.addVertex(drn); 
    int urni = m.addVertex(urn);
    
    const int faceVerts[5][4] = 
    {
        {dlni, drni, urni, ulni}, // 1. new face (head)
        {droi, dloi, dlni, drni}, // 2. bottom connecting face
        {uloi, dloi, dlni, ulni}, // 3. right connecting face
        {uroi, droi, drni, urni}, // 4. left connecting face
        {uroi, uloi, ulni, urni}  // 5. top connecting face
    }; 

    int head = -1;
    unordered_map <int, int> edgeMap;
    for (int fi = 0; fi < 5; fi++)
    {   
        const int faceIdx = m.addface();
        if(head == -1) // head is the first face
            head = faceIdx;
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
    // now we just need to update by calling renderer's upload on mesh
    // ideally we need to delete the old face too

    return head;
}