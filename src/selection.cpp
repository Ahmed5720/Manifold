#include "selection.h"
#include "common.h"

/*
    to shoot a ray from a given pixel on camera we get the pixel pos divide by max w,h
    this gives us a valid x,y, but where does depth come from? depth can be calculated from the view frustums near and far bounds
*/
void Selector::shoot(Camera& cam, Ray& r, float mouseX, float mouseY, float fbWidth, float fbHeight)
{   
    const float aspect = (fbWidth > 0) ? fbWidth / fbHeight : 1.f; 
    // width should depent on actual width not fixed const 
    float ndcX = 2.0f * mouseX / fbWidth - 1.0;
    float ndcY = 1.0f - 2.0f * mouseY / fbHeight; // flip y?

    glm::vec4 nearPoint = glm::vec4(ndcX, ndcY, -1.0, 1.0); 
    glm::vec4 farPoint = glm::vec4(ndcX, ndcY, 1.0, 1.0);

    // now we need to project to world coordinates

    glm::mat4 inverseViewProjection = glm::inverse(cam.projMatrix(aspect) * cam.viewMatrix());

    glm::vec4 worldNear =  inverseViewProjection * nearPoint;
    glm::vec4 worldFar  = inverseViewProjection * farPoint;

    worldNear /= worldNear.w;
    worldFar /= worldFar.w;

    r.origin = worldNear;
    r.dir = normalize(worldFar - worldNear); 
    std::cout << "launched ray at" << mouseX << " " << mouseY << "\n"; 


}
bool Selector::intersect(Ray& r, vector<Vertex>& faceVerts, float& distance, vec3& hit)
{
    // intersect with face's plane and if so check if intersection point is within each edge that is,
    // the point is to the left of each edge defined ccw, that is the dot product is +
    // the problem of ray plane intersection is easier now,
    // t = (A-Po) * N) / (V dot N), for t > 0, A is point on plane, Po is ray origin, N is plane Normal, v is ray direction
    vec3 A = faceVerts[0].position;
    vec3 Po = r.origin;
    vec3 v = r.dir;
    vec3 n = (cross(faceVerts[1].position - faceVerts[0].position, faceVerts[2].position - faceVerts[1].position));
    n /= sqrt(dot(n,n));
    //vec3 n = (faceVerts[0].normal + faceVerts[1].normal + faceVerts[2].normal) / 3.0f;
    float t = dot(A - Po, n) / (dot(v, n));
    if(t > 0.0)
    {   
        hit = Po + r.dir * t; 
        // intersected in front of ray origin with face's plane, now we check if its within the actual face
        /*
        0 -- 1
        |    |
        3 -- 2
        */
        // vector<vec3> edges = {faceVerts[1].position - faceVerts[0].position, faceVerts[2].position - faceVerts[1].position,
        //      faceVerts[3].position - faceVerts[2].position, faceVerts[0].position - faceVerts[3].position};
        
        // for (vec3 e : edges)
        //     if(dot(hit, e) < 0.0)
        //         return false;

        // one other fancy way i could imagine would be to draw a ray on the intersection plane from the intersection point of the plane towards an a vertex of the triangle
        // now if that ray intersects any OTHER edge of the triangle in front of it then it must be outside of it, else then its inside of it.
        // how do we check intersection of two rays on a plane? r1 = o + dt , r2 = o + dt and solve for t? sm like dat. 

        const size_t N = faceVerts.size();
        for (size_t i = 0; i < N; ++i) {
            const vec3& a = faceVerts[i].position;
            const vec3& b = faceVerts[(i + 1) % N].position;
            if (dot(cross(b - a, hit - a), n) < 0.0f) return false;
        }
        distance = glm::distance(hit, Po);         
        return true;
    }
    
    return false;

}