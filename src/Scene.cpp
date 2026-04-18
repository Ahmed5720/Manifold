#include "scene.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
Scene::Scene() : m_cam({4.f, 8.f, -10.f}, -90.f, -20.f)
{}
// {   
//     Scene.m_cam({0.f, 8.f, 20.f}, -90.f, -20.f) // initialze camera position, Feild of view, and ..? 
//     Scene.scene_color = {0.12f, 0.12f, 0.14f, 1.f};
// } 
void Scene::init()
{
    m_renderer.init();
    m_mesh = Mesh::makeCube();
    m_renderer.uploadMesh(m_mesh);
}
void Scene::update(float dt)
{
    m_cam.update(dt);
}
void Scene::draw(int fbWidth, int fbHeight)
{
    const float aspect = (fbHeight > 0) ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight) : 1.f; // here we use static casts as opposed to c-style casts as its type safe

    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(0.12f, 0.12f, 0.14f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const glm::mat4 view = m_cam.viewMatrix();
    const glm::mat4 proj = m_cam.projMatrix(aspect);

    m_renderer.drawGrid(view, proj);
    
    m_renderer.drawMesh(view, proj);
    
    // no depth test for axis, always on top
    m_renderer.drawAxes(view, proj);

}

void Scene::drawUI(float dt) {

    std :: cout << "showing ui\n";
    // Smooth FPS over ~30 frames to avoid jitter
    m_fpsAccum  += (dt > 0.f ? 1.f / dt : 0.f);
    m_fpsFrames += 1;
    if (m_fpsFrames >= 30) {
        m_smoothFps = m_fpsAccum / static_cast<float>(m_fpsFrames);
        m_fpsAccum  = 0.f;
        m_fpsFrames = 0;
    }
 
    // Anchor to the top-left corner with a small margin, no user resize/move
    ImGui::SetNextWindowPos ({10.f, 10.f}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({220.f, 0.f}, ImGuiCond_Always); // height = auto
    ImGui::Begin("Stats", nullptr,
                 ImGuiWindowFlags_NoResize        |
                 ImGuiWindowFlags_NoMove          |
                 ImGuiWindowFlags_NoCollapse      |
                 ImGuiWindowFlags_NoSavedSettings |
                 ImGuiWindowFlags_NoTitleBar      |
                 ImGuiWindowFlags_AlwaysAutoResize);
 
    ImGui::Text("FPS        %5.1f",  m_smoothFps);
    ImGui::Text("Frame      %.2f ms", dt * 1000.f);
    ImGui::Separator();
 
    const int numFaces    = static_cast<int>(m_mesh.faces.size());
    const int numVerts    = static_cast<int>(m_mesh.vertices.size());
    const int numEdges    = static_cast<int>(m_mesh.halfedges.size()) / 2;
 
    ImGui::Text("Vertices   %d", numVerts);
    ImGui::Text("Edges      %d", numEdges);
    ImGui::Text("Faces      %d", numFaces);
    //ImGui::Text("Triangles  %d", numTris);
    ImGui::Separator();
 
    // const Vec3 p = m_camera.position();
    // ImGui::Text("Camera");
    // ImGui::Text("  pos  %.2f  %.2f  %.2f", p.x, p.y, p.z);
    // const Vec3 f = m_camera.forward();
    // ImGui::Text("  fwd  %.2f  %.2f  %.2f", f.x, f.y, f.z);
 
     ImGui::End();
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