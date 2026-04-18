#pragma once
#include "Camera.h"
#include "renderer.h"
#include "mesh.h"
// scene is the single module that is accessed from main, it owns the camera and the renderer as well as the A_mesh
// calls renderer.draw() methods

class Scene{
    public:
        Scene(); 
        void init();
        void update(float dt);
        void draw(int frameBufferWidth, int frameBufferHeight);
        void drawUI(float dt);
        // GLFW callbacks
        void onKey         (int key, int action, int mods);
        void onMouseButton (int button, int action, int mods);
        void onMouseMove   (double xpos, double ypos);
        void onScroll      (double xoffset, double yoffset);
        void onResize      (int width, int height);
        

    private:
        Camera m_cam;
        Renderer m_renderer;
        Mesh m_mesh;

        float m_fpsAccum  = 0.f;
        int   m_fpsFrames = 0;
        float m_smoothFps = 0.f;

};