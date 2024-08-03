#pragma once

#include "Window.h"
#include "glm/vec3.hpp"
#include <cstdint>

namespace engine {
  struct Controlls {
    int32_t moveXPos;
    int32_t moveXNeg;
    int32_t moveYPos;
    int32_t moveYNeg;
    int32_t moveZPos;
    int32_t moveZNeg;

    int32_t condition{GLFW_KEY_UNKNOWN};
  };
}
namespace engine::component {


class Controller {
public:
  Controller(Window &window, const Controlls &controlls) : m_controlls(controlls) , m_window(window){}

  void update(float dt);

  glm::vec3 position() { return m_position; }
  glm::vec3 rotation() { return m_rotation; }

private:
  Window &m_window;
  Controlls m_controlls;

  glm::vec3 m_position;
  glm::vec3 m_rotation;
  glm::vec2 m_prevCursorPos;

  float m_rotationSpeed{0.5f};
  float m_movementSpeed{5.0f};

};

} // namespace engine
