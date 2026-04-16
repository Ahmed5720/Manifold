#include "Renderer.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdio> 


using namespace std;
using namespace glm;
Renderer::Renderer() = default;
Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_gridVAO);
    glDeleteBuffers     (1, &m_gridVBO);
    glDeleteProgram     (m_gridProg);
 
    glDeleteVertexArrays(1, &m_axesVAO);
    glDeleteBuffers     (1, &m_axesVBO);
    glDeleteBuffers     (1, &m_axesCBO);
    glDeleteProgram     (m_axesProg);
}

void Renderer::init()
{
    m_gridProg = loadProgram("shaders/grid.vert", "shaders/grid.frag");
    m_axesProg = loadProgram("shaders/axes.vert", "shaders/axes.frag");
    buildGrid();
    buildAxes();
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