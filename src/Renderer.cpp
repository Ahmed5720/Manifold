#include "Renderer.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio> 
#include <iostream>

using namespace std;
using namespace glm;
Renderer::Renderer() = default;
Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_gridVAO);
    glDeleteBuffers     (1, &m_gridVBO);
    glDeleteProgram     (m_gridProg);
 
    glDeleteVertexArrays(1, &m_axesVAO);
    glDeleteBuffers(1, &m_axesVBO);
    glDeleteBuffers(1, &m_axesCBO);
    glDeleteBuffers(1, &m_meshVBO);
    glDeleteBuffers(1, &m_meshEBO);
    glDeleteVertexArrays(1, &m_meshVAO);
    glDeleteProgram(m_axesProg);
    glDeleteProgram(m_meshProg);

    glDeleteVertexArrays(1, &m_wireVAO);
    glDeleteBuffers     (1, &m_wireVBO);
    glDeleteProgram     (m_wireProg);
}

void Renderer::init()
{
    m_gridProg = loadProgram("shaders/grid.vert", "shaders/grid.frag");
    m_axesProg = loadProgram("shaders/axes.vert", "shaders/axes.frag");
    m_meshProg = loadProgram("shaders/mesh.vert", "shaders/mesh.frag");
    m_wireProg = loadProgram("shaders/wire.vert", "shaders/wire.frag");
    buildGrid();
    buildAxes();


    glGenVertexArrays(1, &m_meshVAO);
    glGenBuffers     (1, &m_meshVBO);
    glGenBuffers     (1, &m_meshEBO);
 
    glBindVertexArray(m_meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER,         m_meshVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshEBO);
    // layout: position(0) vec3 + normal(1) vec3  = 24 bytes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glBindVertexArray(0);


    glGenVertexArrays(1, &m_wireVAO);
    glGenBuffers     (1, &m_wireVBO);
 
    glBindVertexArray(m_wireVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_wireVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glBindVertexArray(0);
}

void Renderer::drawGrid(const mat4& view, const mat4& proj)
{
    glEnable(GL_DEPTH_TEST);
    glUseProgram(m_gridProg);

    setUniformMat4(m_gridProg, "uView", view);
    setUniformMat4(m_gridProg, "uProj", proj);
 
    glBindVertexArray(m_gridVAO);
    glDrawArrays(GL_LINES, 0, m_gridLineCount);
    glBindVertexArray(0);

}

void Renderer::drawAxes(const mat4& view, const mat4& proj) {
    // Axes always render on top — disable depth test for this draw only
    glDisable(GL_DEPTH_TEST);
    glUseProgram(m_axesProg);
    setUniformMat4(m_axesProg, "uView", view);
    setUniformMat4(m_axesProg, "uProj", proj);
 
    glBindVertexArray(m_axesVAO);
    glDrawArrays(GL_LINES, 0, 6); // 3 axes × 2 vertices
    glBindVertexArray(0);
 
    glEnable(GL_DEPTH_TEST);
}

void Renderer::buildGrid() 
{   
    // builds a grid of 64 * 64 units centered at origin with unit length 1.0
    constexpr int HALF = 8;
    constexpr float STEP = 1.0f;
    constexpr int LINES = (HALF * 2 + 1);
    vector<float> verts; // vector of float 3 or 3 vector of size vertices * 3? 
    verts.reserve(LINES * 4 * 2); // since we already know vector size, better to use reserve than push_back
    for (int i = -HALF; i < HALF; i++) {
        const float t = static_cast<float>(i) * STEP;
        // Line parallel to Z
        verts.insert(verts.end(), { t, 0.f, -HALF * STEP, t, 0.f,  HALF * STEP });
        // Line parallel to X
        verts.insert(verts.end(), { -HALF * STEP, 0.f, t, HALF * STEP, 0.f, t });
    }

    m_gridLineCount = static_cast<int>(verts.size() / 3);
 
    glGenVertexArrays(1, &m_gridVAO);
    glGenBuffers     (1, &m_gridVBO);
 
    glBindVertexArray(m_gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_gridVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(verts.size() * sizeof(float)), verts.data(), GL_STATIC_DRAW);
 
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
 
    glBindVertexArray(0);

}

/*
builds three lines of length 4 units from origin representing the axes: x (right vector) in red, y (forward vector) in green, z (up vector) in blue
We'll use two separate VBOs for clarity (positions / colours).
*/
void Renderer::buildAxes()
{
    constexpr float LEN = 16.0f;
 
    const float positions[] = {
        // X axis
        0.f, 0.f, 0.f,   LEN, 0.f, 0.f,
        // Y axis
        0.f, 0.f, 0.f,   0.f, LEN, 0.f,
        // Z axis
        0.f, 0.f, 0.f,   0.f, 0.f, LEN,
    };
 
    const float colours[] = {
        // X  red
        1.f, 0.f, 0.f,   1.f, 0.f, 0.f,
        // Y  green
        0.f, 1.f, 0.f,   0.f, 1.f, 0.f,
        // Z  blue
        0.f, 0.f, 1.f,   0.f, 0.f, 1.f,
    };
 
    glGenVertexArrays(1, &m_axesVAO);
    glBindVertexArray(m_axesVAO);
 
    // Position buffer = attr 0
    glGenBuffers(1, &m_axesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_axesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
 
    // Colour buffer = attr 1
    glGenBuffers(1, &m_axesCBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_axesCBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colours), colours, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
 
    glBindVertexArray(0);
}

// takes a mesh in our mesh datastructure, and builds vbo and ebo from it so it can be uploaded on gpu
// observe that we need to triangulate first (simply fan triangulation before we can build the ebo)
void Renderer::uploadMesh(const Mesh& m)
{
    vector<float> vboData(m.vertices.size() * 6);
    vector<uint32_t> eboData;
    eboData.reserve(m.faces.size() * 6);
    int i = 0;
    for (int vi = 0; vi < m.vertices.size(); vi++)
    {   
        const Vertex& v = m.vertices[vi];
        vboData[i++] = v.position.x;
        vboData[i++] = v.position.y;
        vboData[i++] = v.position.z;
        vboData[i++] = v.normal.x;
        vboData[i++] = v.normal.y;
        vboData[i++] = v.normal.z;
    }

    for (int fi = 0; fi < m.faces.size(); fi++)
    {
        const auto verts = m.faceVertices(fi);
        // fan triangulate 0,1,2 .. 0,2,3 .. 
        for (int i = 1; i < verts.size()-1; i++)
        {
            eboData.push_back((uint32_t)verts[0]);
            eboData.push_back((uint32_t)verts[i]);
            eboData.push_back((uint32_t)verts[i + 1]);
        }
    }

    m_meshTriCount = eboData.size();
    std :: cout << "tri count " << m_meshTriCount << "\n";
    glBindVertexArray(m_meshVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_meshVBO);
    glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(vboData.size() * sizeof(float)), vboData.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(eboData.size() * sizeof(uint32_t)), eboData.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);


     // Wireframe edges
    // Emit one line segment per unique half-edge 
    vector<float> wireData;
    for (int i = 0; i < static_cast<int>(m.halfedges.size()); ++i) {
        const halfEdge& he = m.halfedges[i];
        if (he.twin != -1 && he.twin < i) continue;
 
        const vec3& a = m.vertices[he.vertex].position;
        const vec3& b = m.vertices[m.halfedges[he.next].vertex].position;
        wireData.insert(wireData.end(), {a.x, a.y, a.z, b.x, b.y, b.z});
    }
 
    m_wireVertCount = static_cast<int>(wireData.size() / 3);
 
    glBindVertexArray(m_wireVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_wireVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(wireData.size() * sizeof(float)), wireData.data(), GL_DYNAMIC_DRAW);
    glBindVertexArray(0);


}
void Renderer::drawMesh(const mat4& view, const mat4& proj)
{   
  
    if(m_meshTriCount == 0)
        return;
    glEnable(GL_DEPTH_TEST);
    glUseProgram(m_meshProg);
    setUniformMat4(m_meshProg, "uView", view);
    setUniformMat4(m_meshProg, "uProj", proj);

    glBindVertexArray(m_meshVAO);
    glDrawElements(GL_TRIANGLES, m_meshTriCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);


    //draw wireframe
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.f, -1.f);
 
    glUseProgram(m_wireProg);
    setUniformMat4(m_wireProg, "uView", view);
    setUniformMat4(m_wireProg, "uProj", proj);
 
    glBindVertexArray(m_wireVAO);
    glDrawArrays(GL_LINES, 0, m_wireVertCount);
    glBindVertexArray(0);
 
    glDisable(GL_POLYGON_OFFSET_LINE);


}


//shader helpers
 
static std::string readFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open shader: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
 
uint32_t Renderer::compileShader(uint32_t type, const std::string& src) {
    const uint32_t id  = glCreateShader(type);
    const char*    ptr = src.c_str();
    glShaderSource(id, 1, &ptr, nullptr);
    glCompileShader(id);
 
    int ok = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        glDeleteShader(id);
        throw std::runtime_error(std::string("Shader compile error:\n") + log);
    }
    return id;
}
 
uint32_t Renderer::linkProgram(uint32_t vert, uint32_t frag) {
    const uint32_t id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);
    glLinkProgram(id);
 
    int ok = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512];
        glGetProgramInfoLog(id, 512, nullptr, log);
        glDeleteProgram(id);
        throw std::runtime_error(std::string("Program link error:\n") + log);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return id;
}
 
uint32_t Renderer::loadProgram(const std::string& vertPath,
                                const std::string& fragPath) {
    const uint32_t v = compileShader(GL_VERTEX_SHADER,   readFile(vertPath));
    const uint32_t f = compileShader(GL_FRAGMENT_SHADER, readFile(fragPath));
    return linkProgram(v, f);
}
 
void Renderer::setUniformMat4(uint32_t prog, const char* name, const mat4& m) {
    const int loc = glGetUniformLocation(prog, name);
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(m));
}
 
void Renderer::setUniformVec3(uint32_t prog, const char* name, vec3 v) {
    const int loc = glGetUniformLocation(prog, name);
    if (loc != -1)
        glUniform3fv(loc, 1, value_ptr(v));
}