#include "selection.h"
typedef MAX_SELECTION_DIST 100.0
void Selector::shoot(Ray& r, float x, float y)
{
    r.origin = {x,y,0};
    
}
bool Selector::intersect(Ray& r, Face& f, float& distance)
{

}
void Selector::setSelected(Face& f)
{

}
bool Selector::select(Mesh& m, float x, float y)
{
    Ray r;
    float bestDist = MAX_SELECTION_DIST;
    face bestFace;
    face f;
    bool intersected = false; 
    float t = 0;
    shoot(r, x, y);
    // for all o in objects
        for (int i = 0; i < m.faces.size(); i++)
            f = m.faces[i];
            intersected = intersect(r, f, t);
            if(intersected && t < bestDist)
            {
                bestDist = t;
                bestFace = f;
            }
    if(intersected)
        setSelected(bestFace);
    return intersected;
} 