#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <GLFW/glfw3.h>
class Camera;

class FreeCameraController {
    public:
        void handleInput(GLFWwindow* window,const Camera& camera);
        void update(Camera& camera, float dt);

        void setYawPitch(float yaw, float pitch);

        float getYaw() const { return freeCameraYaw; }
        float getPitch() const { return freeCameraPitch; }
    
    private:
        float freeCameraYaw{0.f};
        float freeCameraPitch{0.f};

        glm::vec3 moveVelocity{};
        glm::vec3 moveSpeed{10.f, 10.f, 10.f};

        float rotateSpeed{1.f};
        glm::vec2 rotationVelocity{};
};