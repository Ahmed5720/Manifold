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
    vec3 fnorm = m.faceNormal(faceIdx);
    vector<int> oldVerts = m.faceVertices(faceIdx); 


    // 1. grab oldFace data

    vector<int> newVerts;
    for (int i = 0; i < 4; i++)
    {
        vec3 pos = m.vertices[oldVerts[i]].position + offset * fnorm;
        // what about v.norm? should probably copy from old v?
        newVerts.push_back(m.addVertex(pos));
    }

    vector<int> oldHalfedges;
    m.forEachHalfEdgeofFace(m.faces[faceIdx].halfedge,
    [&](int h)
    {
        oldHalfedges.push_back(h); 
    }
    );
    
    // 2. construct cap
    int cap = m.addface();
    int topHe, capHe, topV, capV;
    vector<int> capHalfEdges;
    vector<int> topHalfEdges;
    // because the cap's normal must be the same as the old face, then its winding should be the same
    // whereas the top's winding should be reversed i.e cap = verts[i], top = verts[i+1 % 4]
    for (int i = 0; i < 4; i++)
    {
        topHe = m.addHalfEdge();
        capHe = m.addHalfEdge();
        topV = newVerts[(i+1) % 4];
        capV = newVerts[i];

        m.halfedges[topHe].vertex = topV;
        m.halfedges[capHe].vertex = capV;
        m.halfedges[topHe].twin = capHe;
        m.halfedges[capHe].twin = topHe;
        m.vertices[topV].halfedge = topHe;
        m.vertices[capV].halfedge = capHe;

        topHalfEdges.push_back(topHe);
        capHalfEdges.push_back(capHe);
    }    

    m.linkFaceLoop(cap, capHalfEdges);


    // 3. construct 4 side faces
    /*
     old verts and new verts have the same winding direction since they are copies
     that direction is the direction of the face, pointing outwards
     each side face consists of oldhalfedge, up, tophalfedge, down
     since oldhalfedge vertex is the first vertex of old face, up must start with the next
     for newVerts
     newv0<-----newv1
     :             ^
     : down        : up
     :             : 
     v             :
     oldv0----->oldv1
    */
    vector<int> upHalfEdges;
    vector<int> downHalfEdges;
    for (int i = 0; i < 4; i++)
    {   
        vector<int> sideHe;
        int sideFace = m.addface();
        int startHe = oldHalfedges[i];
        int up = m.addHalfEdge();
        int down = m.addHalfEdge();
        m.halfedges[up].vertex = oldVerts[(i+1) % 4];
        m.halfedges[down].vertex = newVerts[i];
        int top = topHalfEdges[i];
        sideHe = {startHe, up, top, down};
        upHalfEdges.push_back(up);
        downHalfEdges.push_back(down);
        m.linkFaceLoop(sideFace, sideHe);
    }

    // 4. now stitching the sidefaces
    /*
    
    <----------  
    :           ^ :         ^
    : dn1   up1 : : dn0     : up0
    :  side1    : :   side2 : 
    :           : :         :
    v           : v -------->
    */
    for (int i = 0; i < 4; i++)
    {   
        int up = upHalfEdges[i];
        int down = downHalfEdges[(i+1) % 4];
        m.halfedges[up].twin = down;
        m.halfedges[down].twin = up;
    }

    // currently this does nothing. its not clear to me if it will ever be useful but for now well keep it.
    m.faces[cap].alive = false;
    m.computeNormals();

    return cap;
}