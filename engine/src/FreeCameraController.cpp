#include "FreeCameraController.h"

#include <Graphics/Camera.h>
#include <util/InputUtil.h>

void FreeCameraController::handleInput(GLFWwindow* window, const Camera& camera) {
  const auto camFront = camera.getTransform().getLocalFront();
  const auto camRight = camera.getTransform().getLocalRight();

  glm::vec3 moveVector{};
  if (util::isKeyPressed(window, GLFW_KEY_W)) {
    moveVector += camFront;
  }
  if (util::isKeyPressed(window, GLFW_KEY_S)) {
    moveVector -= camFront;
  }
  if (util::isKeyPressed(window, GLFW_KEY_A)) {
    moveVector -= camRight;
  }
  if (util::isKeyPressed(window, GLFW_KEY_D)) {
    moveVector += camRight;
  }
  if (util::isKeyPressed(window, GLFW_KEY_Q)) {
    moveVector -= math::GlobalUpAxis / 2.f;
  }
  if (util::isKeyPressed(window, GLFW_KEY_E)) {
    moveVector += math::GlobalUpAxis / 2.f;
  }

  moveVelocity = moveVector * moveSpeed;
  if (util::isKeyPressed(window, GLFW_KEY_LEFT_SHIFT)) {
    moveVelocity *= 2.f;
  }

  rotationVelocity = glm::vec2(0.0f);
  if (util::isKeyPressed(window, GLFW_KEY_LEFT)) {
    rotationVelocity.x = -rotateSpeed;
  }
  if (util::isKeyPressed(window, GLFW_KEY_RIGHT)) {
    rotationVelocity.x = rotateSpeed;
  }
  if (util::isKeyPressed(window, GLFW_KEY_UP)) {
    rotationVelocity.y = rotateSpeed;
  }
  if (util::isKeyPressed(window, GLFW_KEY_DOWN)) {
    rotationVelocity.y = -rotateSpeed;
  }
}

void FreeCameraController::update(Camera& camera, float dt)
{
  auto newPos = camera.getPosition();
  newPos += moveVelocity * dt;
  camera.setPosition(newPos);

  freeCameraYaw += rotationVelocity.x * dt;
  freeCameraPitch += rotationVelocity.y * dt;
  camera.setYawPitch(freeCameraYaw, freeCameraPitch);
}

void FreeCameraController::setYawPitch(float yaw, float pitch)
{
  freeCameraYaw = yaw;
  freeCameraPitch = pitch;
}