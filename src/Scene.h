#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "renderer.h"
#include "mesh.h"
#include "common.h"
// scene is the single module that is accessed from main, it owns the camera and the renderer as well as the A_mesh
// calls renderer.draw() methods

class Selector;
class Scene{
    public:
        Camera m_cam;   
        Mesh m_mesh;
        Renderer m_renderer;
        Scene(); 
        void init(GLFWwindow* window);
        void update(float dt);
        void draw(int frameBufferWidth, int frameBufferHeight);
        void drawUI(float dt);
        // GLFW callbacks
        void onKey         (int key, int action, int mods);
        void onMouseButton (int button, int action, int mods);
        void onMouseMove   (double xpos, double ypos);
        void onScroll      (double xoffset, double yoffset);
        void onResize      (int width, int height);
        bool select(float x, float y);
        

    private:
        Selector* m_selector;
        GLFWwindow* m_window; // not safe
        float m_fpsAccum  = 0.f;
        int   m_fpsFrames = 0;
        float m_smoothFps = 0.f;

};