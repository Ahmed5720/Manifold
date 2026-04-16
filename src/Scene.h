#pragma once
#include "Camera.h"
#include "renderer.h"

// scene is the single module that is accessed from main, it owns the camera and the renderer as well as the A_mesh
// calls renderer.draw() methods

class Scene{
    public:
        Scene(); 
        void init();
        void update();
        void draw(int frameBufferWidth, int frameBufferHeight);
        // GLFW callbacks
        void onKey         (int key, int action, int mods);
        void onMouseButton (int button, int action, int mods);
        void onMouseMove   (double xpos, double ypos);
        void onScroll      (double xoffset, double yoffset);
        void onResize      (int width, int height);

    private:
        Camera m_cam;
        Renderer m_renderer;
        glm::vec3 scene_color;

};