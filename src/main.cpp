#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <cstdio>
#include "Scene.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Forward declarations of GLFW callbacks
static void cbKey        (GLFWwindow*, int, int, int, int);
static void cbMouseButton(GLFWwindow*, int, int, int);
static void cbMouseMove  (GLFWwindow*, double, double);
static void cbScroll     (GLFWwindow*, double, double);
static void cbResize     (GLFWwindow*, int, int);
static void cbGLError    (GLenum, GLenum, GLuint, GLenum, GLsizei,
                          const GLchar*, const void*);

GLFWwindow* initGLFW()
{
    if (!glfwInit())
        throw std::runtime_error("glfwInit failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // required on macOS
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Manifold", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("glfwCreateWindow failed");
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    return window;
}
void initGlad()
{
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("gladLoadGLLoader failed");

    std::printf("OpenGL %s — %s\n", glGetString(GL_VERSION),
                                     glGetString(GL_RENDERER));


}   

void initIMGUI(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, /*install_callbacks=*/true);
    ImGui_ImplOpenGL3_Init("#version 410");

    // if (GLAD_GL_ARB_debug_output) {
    //     glEnable(GL_DEBUG_OUTPUT);
    //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //     glDebugMessageCallback(cbGLError, nullptr);
    //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    // }
}
int main() {

    GLFWwindow* window = initGLFW();
    initGlad();
    initIMGUI(window); 

    Scene scene;
    scene.init();

    // Store scene pointer in window so callbacks can reach it
    glfwSetWindowUserPointer(window, &scene);
    glfwSetKeyCallback        (window, cbKey);
    glfwSetMouseButtonCallback(window, cbMouseButton);
    glfwSetCursorPosCallback  (window, cbMouseMove);
    glfwSetScrollCallback     (window, cbScroll);
    glfwSetFramebufferSizeCallback(window, cbResize);

    double prevTime = glfwGetTime();
    // main loop
    while (!glfwWindowShouldClose(window)) {
        const double now = glfwGetTime();
        const float  dt  = static_cast<float>(now - prevTime);
        prevTime = now;

        glfwPollEvents();

        // Close on Escape
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        int fbW = 0, fbH = 0;
        glfwGetFramebufferSize(window, &fbW, &fbH);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        scene.update(dt);
        scene.draw(fbW, fbH);
        scene.drawUI(dt);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}


static Scene* getScene(GLFWwindow* w) {
    return static_cast<Scene*>(glfwGetWindowUserPointer(w));
}

static void cbKey(GLFWwindow* w, int key, int /*scan*/, int action, int mods) {
    getScene(w)->onKey(key, action, mods);
}
static void cbMouseButton(GLFWwindow* w, int btn, int action, int mods) {
    getScene(w)->onMouseButton(btn, action, mods);
}
static void cbMouseMove(GLFWwindow* w, double x, double y) {
    getScene(w)->onMouseMove(x, y);
}
static void cbScroll(GLFWwindow* w, double xo, double yo) {
    getScene(w)->onScroll(xo, yo);
}
static void cbResize(GLFWwindow* w, int width, int height) {
    getScene(w)->onResize(width, height);
}

static void cbGLError(GLenum /*src*/, GLenum type, GLuint /*id*/,
                      GLenum severity, GLsizei /*len*/,
                      const GLchar* msg, const void* /*user*/) {
    if (type == GL_DEBUG_TYPE_ERROR)
        std::fprintf(stderr, "[GL ERROR] severity=0x%x  %s\n", severity, msg);
    else
        std::fprintf(stderr, "[GL WARN]  severity=0x%x  %s\n", severity, msg);
}