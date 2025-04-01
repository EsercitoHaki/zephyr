#include "InputUtil.h"

namespace util
{
bool isKeyPressed(GLFWwindow* window, int key)
{
  return glfwGetKey(window, key) == GLFW_PRESS;
}

glm::vec2 getStickState(GLFWwindow* window, const StickBindings& bindings)
{
  glm::vec2 stick{};
  if (isKeyPressed(window, bindings.up)) {
    stick.y -= 1.f;
  }
  if (isKeyPressed(window, bindings.down)) {
    stick.y += 1.f;
  }
  if (isKeyPressed(window, bindings.left)) {
    stick.x -= 1.f;
  }
  if (isKeyPressed(window, bindings.right)) {
    stick.x += 1.f;
  }
  return stick;
}

}