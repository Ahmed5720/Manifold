#include "scene.h"
#include <glm/glm.hpp>
#include <imgui.h>
#include <iostream>
#include "selection.h"
#include "extrude.h"
Scene::Scene() : m_cam({4.f, 8.f, -10.f}, -90.f, -20.f)
{}
// {   
//     Scene.m_cam({0.f, 8.f, 20.f}, -90.f, -20.f) // initialze camera position, Feild of view, and ..? 
//     Scene.scene_color = {0.12f, 0.12f, 0.14f, 1.f};
// } 
void Scene::init(GLFWwindow* window)
{   
    m_window = window;
    m_renderer.init();
    m_mesh = Mesh::makeCube();
    m_renderer.uploadMesh(m_mesh);
    m_selector = new Selector();
    m_extruder = new Extruder();
}
void Scene::update(float dt)
{
    m_cam.update(dt);
}
void Scene::draw(int fbWidth, int fbHeight)
{
    const float aspect = (fbHeight > 0) ? static_cast<float>(fbWidth) / static_cast<float>(fbHeight) : 1.f; 
    fbW = fbWidth;
    fbH = fbHeight;
    glViewport(0, 0, fbWidth, fbHeight);
    glClearColor(0.12f, 0.12f, 0.14f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = m_cam.viewMatrix();
    proj = m_cam.projMatrix(aspect);

    m_renderer.drawGrid(view, proj);
    
    m_renderer.drawMesh(view, proj);
    
    // no depth test for axis, always on top
    m_renderer.drawAxes(view, proj);

    m_renderer.drawDebugPoint(view,proj);

}

void Scene::drawUI(float dt) {

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
    char axis = activeAxis == 0? 'X' : activeAxis == 1? 'Y' : activeAxis == 2? 'Z' : 'U'; 
    ImGui::Text("Active Translation Axis %c", axis);
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
    if(key == GLFW_KEY_UP)
    {
        translate(1.0);
        m_renderer.uploadMesh(m_mesh);
        cout << "translate up\n"; 
    }
    else if(key == GLFW_KEY_DOWN)
    {
        translate(-1.0);
        m_renderer.uploadMesh(m_mesh);
        cout << "translate down\n";
    }
    if(key == GLFW_KEY_X)
        activeAxis = 0;
    if(key == GLFW_KEY_Y)
        activeAxis = 1;
    if(key == GLFW_KEY_Z)
        activeAxis = 2;
    int selection = -1;
    if(key == GLFW_KEY_L)
    {   
        selection = m_selector->activeSelectedFace;
        if(selection != 0)
        {
            int newFace = m_extruder->extrude(*this, selection);
            m_mesh.faces[selection].selected = false;
            m_mesh.faces[newFace].selected = true; 
            m_renderer.uploadMesh(m_mesh);
            selection = -1; //
            cout << "extruded\n"; 
        }
    }

        
}
 
void Scene::onMouseButton(int button, int action, int mods) {
    m_cam.onMouseButton(button, action, mods);
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double mouseX, mouseY;
        glfwGetCursorPos(m_window, &mouseX, &mouseY);
        if(select(mouseX, mouseY))
            std::cout<<" selected\n";
        else
            std::cout<<"nothing to select\n";
    }

}
 
void Scene::translate(float dir)
{
    if(selectedCount == 0)
        return;
    vec3 translation = {dir * TRANS_SENSITIVITY * (activeAxis == 0), dir * TRANS_SENSITIVITY * (activeAxis == 1), dir * TRANS_SENSITIVITY * (activeAxis == 2)};

    // if we can put the selected faces first, then the unselected, then we would be able to just loop over selected faces instead 
    // of over all faces, then renderer does smth similar so maybe we can use that in some way? 
    vector<int> vertexIndices;
    for (int i= 0; i < m_mesh.faces.size(); i++)
        if(m_mesh.faces[i].selected)
        {
            vertexIndices = m_mesh.faceVertices(i);
            for(int j = 0; j < vertexIndices.size(); j++)
                m_mesh.vertices[vertexIndices[j]].position += translation;
        }
}
void Scene::onMouseMove(double xpos, double ypos) {
    m_cam.onMouseMove(xpos, ypos);
}
 
void Scene::onScroll(double xoffset, double yoffset) {
    m_cam.onScroll(xoffset, yoffset);
}
 
void Scene::onResize(int /*width*/, int /*height*/) {
    //  viewport is set each frame in draw()
}

bool Scene::select(float x, float y)
{
    Ray r;
    Mesh& m = m_mesh;
    Camera& cam = m_cam;
    vec3 hitPoint;
    float bestDist = MAX_SELECTION_DIST;
    int bestFace = -1;
    bool intersected = false; 
    float t = 0;
    m_selector->shoot(cam, r, x, y, fbW, fbH);
    // for all o in objects
        for (int i = 0; i < m.faces.size(); i++)
        {
            vector<Vertex> faceVerts = m.faceVerts(i);
            bool valid = m_selector->intersect(r, faceVerts,t, hitPoint);
            if(valid)
                intersected = true;
            std::cout << "intersected?" << intersected << "\n";
            if(valid && t < bestDist)
            {
                bestDist = t;
                bestFace = i;
            }
        }
    if(intersected)
    {   
        bool old = m.faces[bestFace].selected;
        m.faces[bestFace].selected = !old;
        m_selector->activeSelectedFace = bestFace;
        if(!old)
            selectedCount++;
        else
            selectedCount--;
        cout << "number of selected faces" << selectedCount << "\n";
        m_renderer.updateSelection(m, hitPoint, view, proj); 
    }
    return intersected;
} 