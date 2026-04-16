#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
class Camera{
    public:
        Camera(vec3 position = {0.f, 8.f, 16.f}, float yawDeg = -90.0f, float pitchDeg = -20.0f);

        void update(float dt);

        void onKey         (int key, int action, int mods);
        void onMouseButton (int button, int action, int mods);
        void onMouseMove   (double xpos, double ypos);
        void onScroll      (double xoffset, double yoffset);

        mat4 viewMatrix() const; 
        mat4 projMatrix(float aspectRatio) const;

        vec3 position() const{return m_pos;}
        vec3 forward() const;
        float fovDeg() const{return m_fov;}

        float speed = 10.0f;
        float mouseSens = 0.12f;
    private:
        void updateVectors();

        vec3  m_pos;
        float m_yaw;    // degrees, -90 = looking along -Z
        float m_pitch;  // degrees
        float m_fov;    // vertical field-of-view in degrees
    
        // derived direction vectors that change whenever yaw/pitch change
        vec3 m_front;
        vec3 m_right;
        vec3 m_up;
    
        // Mouse look state
        bool  m_mouseCapture = false;
        float m_lastX = 0.f;
        float m_lastY = 0.f;
        bool  m_firstMouse = true;
    
        // Key state (true = currently pressed)
        bool m_keys[512] = {};
};