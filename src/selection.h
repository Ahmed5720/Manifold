#include "scene.h"
/*

selection should be simple, lets assume we only do face selection.
first we need a method to project from mouse to 3d coord, then a way to identify that a point is on the face,
if so set it selected, then a way to visualize the selected face, by changing its color

so lets have 3 routines

    shoot()
    intersect()
    selectFace()

    we also need a ray struct
*/

struct Ray
{
    glm::vec3 origin;
    glm::vec3 dir;
    float t;
};

class Selector
{
    private:

    public:
        void shoot(Camera& cam, Ray& r, float x, float y); // creates the ray from screen pos x,y
        bool intersect(Ray& r, vector<Vertex>& faceVerts, float& distance); // tests intersection of ray r with face f
        void setSelected(Face& f); // sets f to selected and changes its color too
        //bool select(Scene& scene, float mouseX, float mouseY);  
}; 