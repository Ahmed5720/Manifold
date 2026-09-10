/*
we need functionality for face extrusion, pressing e after a face is selected should duplicate this face and draw 4 new edges connecting the old face with the new one
would also be good if the new built face is offset by some value on the faces direction.finally the old face should ideally be deleted.


to construct a new face we copy the 4 vertices and push them forward
now we create 5 new faces in ccw order. 

now using the same routine in makeCube() we construct half edges
and again using the same routine in makeCube we stith these half edges


*/

#include "scene.h"

class Extruder
{
    public:
       // Extruder();
        int extrude(Scene& scene, int faceIdx);
    private:
        // we shall see
        float offset = 0.3f;

}; 