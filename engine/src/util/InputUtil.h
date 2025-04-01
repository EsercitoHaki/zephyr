#pragma once

#include <glm/vec2.hpp>

#include <GLFW/glfw3.h>

namespace util
{
bool isKeyPressed(GLFWwindow* window, int key);

struct StickBindings {
    int up;
    int down;
    int left;
    int right;
};

glm::vec2 getStickState(GLFWwindow* window, const StickBindings& bindings);
}