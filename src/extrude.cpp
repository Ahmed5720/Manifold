#include "extrude.h"

// later extruder should specify the mesh we are extruding, however currently
// we simply have a scene and the scene, contains a single mesh, so we just pass
// that around 
/*

the other thing that extrusion needs to do is to deselect the old face and instead select the new face

naming it turns out can bepretty hard, lets denote the vertices of the face we extrude
from as uloi = upper left old index 


lets try to divide this problem into a series of logical steps
we want to grab a face, copy its vertices along the face's normal, and correctly update its half edges, and faces,
the trick is that we need to grab the half edges of the face (before extrusion) and reuse them as half edges of the new side faces that will connect the mesh with the new face.
to make it make sense lets assume we are extruding upwards, which means we need 4 vertical edges (one up and one down for each) and a cap (the new face) (and 4 horizontal edges the cap sits on)
1. we copy the old vertices oldVs into a "newVs", with positions updated along normal, and newV[i].halfedge reset to -1
2. we grab the half edges belonging to the old face into oldHe
3. we construct 4 half edges of the new face lets call that face (cap), and update the vertex of the each half edge, and the half edge of each vertex correctly\
4. link the 4 half edges of the new cap face
5. now we can create the top 4 half edges (on which the cap will sit) for each half edge of the top, the twin comes from the cap
6. create the 4 side edges that connect old face with new face, for each edge we create two half edges one UP one Down, and appropriately set the vertices
7. finally add the 4 side faces, 
we already have the half edges for them, the trick is to identify the right half edges.
these half edges are the old half edge (from old face),
the half edge extending from it (up), 
the (top) half edge,
and finally the half edge returning from it (down)
we just need to do addfaces and link them
*/
int Extruder::extrude(Scene& scene, int faceIdx)
{
    Mesh& m = scene.m_mesh;
    Face& f = m.faces[faceIdx];
    vec3 fnorm = {0,0,1}; // for now 
    vector<int> verts = m.faceVertices(faceIdx); 

    // there's no reason to assume this is the actual order faceVerts gets vertices by 
    // starting at face's half edge's vertex, whichever that vertex is.
    vec3 uln = m.vertices[verts[0]].position + offset * fnorm; // upper left new vert
    vec3 dln = m.vertices[verts[1]].position + offset * fnorm;
    vec3 drn = m.vertices[verts[2]].position + offset * fnorm;
    vec3 urn = m.vertices[verts[3]].position + offset * fnorm;

    // we need to reset face's half edges to null so that they stop being paired
    // so the new side faces can pair their half edges with the old edges
    // i wonder if that would work

    
    // store he indices of deleted face since we could reuse
    // them later on the sidefaces. how do we identify which side face's half edge?
    int h0 = m.vertices[verts[0]].halfedge;
    int h1 = m.vertices[verts[1]].halfedge;
    int h2 = m.vertices[verts[2]].halfedge;
    int h3 = m.vertices[verts[3]].halfedge;
    
    // reset half edges
    // should we access each halfedge, get its twin, access that half edge (the twin) then set its twin to -1?
    for (int i = 0; i < 4; i++)
        m.vertices[verts[i]].halfedge = -1;
    


    // should we delete these 4 vertices? we no longer need anything from that face i believe
    // but deleting a vertex messes with the indices of the others? 

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

    return head;
}