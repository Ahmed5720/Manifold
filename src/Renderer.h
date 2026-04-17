#pragma once
#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <glm/gtc/type_ptr.hpp>

// owns openGL objects (VAOs, VBOS and Shader programs)
// a caller passes matrices and the renderer issues the drawcalls
using namespace glm;
class Renderer{
    public:
        Renderer();
        ~Renderer();

        void init(); 

        void drawGrid(const mat4& view, const glm::mat4& proj);
        void drawAxes(const mat4& view, const glm::mat4& proj);
        void drawMesh(const mat4& view, const mat4& proj);
        void uploadMesh(const Mesh&  mesh);
    private:
        uint32_t compileShader (uint32_t type, const std::string& src);
        uint32_t linkProgram   (uint32_t vert, uint32_t frag);
        uint32_t loadProgram   (const std::string& vertPath, const std::string& fragPath);
        void     setUniformMat4(uint32_t prog, const char* name, const mat4& m);
        void     setUniformVec3(uint32_t prog, const char* name, vec3 v);

        // grid
        uint32_t m_gridVAO   = 0;
        uint32_t m_gridVBO   = 0;
        uint32_t m_gridProg  = 0;
        int      m_gridLineCount = 0;
    
        // axes
        uint32_t m_axesVAO  = 0;
        uint32_t m_axesVBO  = 0;  // positions
        uint32_t m_axesCBO  = 0;  // colours
        uint32_t m_axesProg = 0;
    
        void buildGrid();
        void buildAxes();

        // Mesh
        uint32_t m_meshVAO      = 0;
        uint32_t m_meshVBO      = 0;  // interleaved: position(3) + normal(3)
        uint32_t m_meshEBO      = 0;  // triangle indices
        uint32_t m_meshProg     = 0;
        uint32_t m_wireVAO      = 0;
        uint32_t m_wireVBO      = 0;  // edge line endpoints
        uint32_t m_wireProg     = 0;
        int      m_meshTriCount = 0;
        int      m_wireVertCount = 0;
    };