#include "camera.h"
#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstring>

using namespace glm;
Camera::Camera(vec3 position, float yawDeg, float pitchDeg)
    : m_pos(position)
    , m_yaw(yawDeg)
    , m_pitch(pitchDeg)
    , m_fov(60.f)
{   

    // hmmmhmm unnessicary jargon
    std::memset(m_keys, 0, sizeof(m_keys));
    updateVectors();
}



void Camera::update(float dt) {
    const float dist = speed * dt;

    // WASD moves in the horizontal plane (ignore m_front's Y component so the
    // camera doesn't drift up/down when looking at an angle)
    const vec3 flatForward = normalize(vec3(m_front.x, 0.f, m_front.z));
    const vec3 flatRight   = normalize(vec3(m_right.x, 0.f, m_right.z));

    if (m_keys[GLFW_KEY_W]) m_pos += flatForward * dist;
    if (m_keys[GLFW_KEY_S]) m_pos -= flatForward * dist;
    if (m_keys[GLFW_KEY_A]) m_pos -= flatRight   * dist;
    if (m_keys[GLFW_KEY_D]) m_pos += flatRight   * dist;
    if (m_keys[GLFW_KEY_Q]) m_pos.y -= dist;
    if (m_keys[GLFW_KEY_E]) m_pos.y += dist;
}

// GLFW callbacks

void Camera::onKey(int key, int action, int /*mods*/) {
    if (key < 0 || key >= 512) return;
    if (action == GLFW_PRESS)   m_keys[key] = true;
    if (action == GLFW_RELEASE) m_keys[key] = false;
}

void Camera::onMouseButton(int button, int action, int /*mods*/) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        m_mouseCapture = (action == GLFW_PRESS);
        if (m_mouseCapture) m_firstMouse = true;
    }
}

void Camera::onMouseMove(double xpos, double ypos) {
    if (!m_mouseCapture) return;

    if (m_firstMouse) {
        m_lastX     = static_cast<float>(xpos);
        m_lastY     = static_cast<float>(ypos);
        m_firstMouse = false;
        return;
    }

    const float dx =  (static_cast<float>(xpos) - m_lastX) * mouseSens;
    const float dy = -(static_cast<float>(ypos) - m_lastY) * mouseSens; // Y flipped
    m_lastX = static_cast<float>(xpos);
    m_lastY = static_cast<float>(ypos);

    m_yaw   += dx;
    m_pitch  = std::clamp(m_pitch + dy, -89.f, 89.f);
    updateVectors();
}

void Camera::onScroll(double /*xoffset*/, double yoffset) {
    m_fov = std::clamp(m_fov - static_cast<float>(yoffset) * 2.f, 10.f, 90.f);
}


mat4 Camera::viewMatrix() const {
    return lookAt(m_pos, m_pos + m_front, m_up);
}

mat4 Camera::projMatrix(float aspectRatio) const {
    return perspective(m_fov, aspectRatio, 0.1f, 1000.f);
}


vec3 Camera::forward() const { return m_front; }


void Camera::updateVectors() {
    const float yawR   = glm::radians(m_yaw);
    const float pitchR = glm::radians(m_pitch);

    m_front = normalize(vec3(
        std::cos(yawR) * std::cos(pitchR),
        std::sin(pitchR),
        std::sin(yawR) * std::cos(pitchR)
    ));
    m_right = normalize(cross(m_front, vec3(0.f, 1.f, 0.f)));
    m_up    = normalize(cross(m_right, m_front));
}