#include "scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
Scene::Scene() : m_cam({0.f, 8.f, 20.f}, -90.f, -20.f)
{}
// {   
//     Scene.m_cam({0.f, 8.f, 20.f}, -90.f, -20.f) // initialze camera position, Feild of view, and ..? 
//     Scene.scene_color = {0.12f, 0.12f, 0.14f, 1.f};
// } 
void Scene::init()
{
    m_renderer.init();
}
void Scene::update(float dt)
{
    m_cam.update(dt);
}
void Scene::draw(int frameBufferWidth, int frameBufferHeight)
{
    const float aspect = (fbHeight > 0) ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight) : 1.f; // here we use static casts as opposed to c-style casts as its type safe

    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(scene_color);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = m_cam.viewMatrix();
    const glm::mat4 proj = m_cam.projMatrix(aspect);

    m_renderer.drawGrid(view, proj);
    
    // no depth test for axis, always on top
    m_renderer.drawAxes(view, proj);

}


// GLFW callback forwarders
void Scene::onKey(int key, int action, int mods) {
    m_cam.onKey(key, action, mods);
}
 
void Scene::onMouseButton(int button, int action, int mods) {
    m_cam.onMouseButton(button, action, mods);
}
 
void Scene::onMouseMove(double xpos, double ypos) {
    m_cam.onMouseMove(xpos, ypos);
}
 
void Scene::onScroll(double xoffset, double yoffset) {
    m_cam.onScroll(xoffset, yoffset);
}
 
void Scene::onResize(int /*width*/, int /*height*/) {
    // Nothing to do yet; viewport is set each frame in draw()
}