#include "Game.h"
#include "Graphics/GPUMesh.h"
#include "glm/ext/matrix_float4x4.hpp"

#include <GLFW/glfw3.h>

#include <chrono>
#include <cmath>
#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <Graphics/Scene.h>
#include <stdexcept>
#include <thread>

namespace {
    static constexpr std::uint32_t SCREEN_WIDTH = 1280;
    static constexpr std::uint32_t SCREEN_HEIGHT = 960;
    static constexpr auto NO_TIMEOUT = std::numeric_limits<std::uint64_t>::max();
}

void Game::init()
{
    if (!glfwInit()){
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        glfwTerminate();
        std::exit(1);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(
        //size
        SCREEN_WIDTH,
        SCREEN_HEIGHT,

        "Vulkan app",
        nullptr,
        nullptr
    );

    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        std::exit(1);
    }

    renderer.init(window, vSync);

    {
        const auto scene = renderer.loadScene("assets/models/kitchen/kitchen.gltf");
        createEntitiesFromScene(scene);
    }

    {
        // const auto scene = renderer.loadScene("assets/models/knight/result.gltf");
        // createEntitiesFromScene(scene);

        // std::cout << "Các thực thể đã tạo:" << std::endl;
        // for (const auto& ePtr : entities) {
        //     std::cout << "- " << ePtr->tag << std::endl;
        // }

        // const glm::vec3 catoPos{5.f, 5.f, 0.f};
        // auto& cato = findEntityByName("arrow");
        // cato.transform.position = catoPos;
    }

    {
        static const float zNear = 1.f;
        static const float zFar = 1000.f;
        static const float fovX = glm::radians(45.f);
        static const float aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

        camera.init(fovX, zNear, zFar, aspectRatio);

        const auto startPos = glm::vec3{8.9f, 4.09f, 8.29f};
        camera.setPosition(startPos);
    }

    cameraController.setYawPitch(-2.5f, 0.2f);

    sunlightDir = glm::vec4{0.371477008, 0.470861048, 0.80018419, 0.f};
    sunlightColorAndIntensity = glm::vec4{213.f / 255.f, 136.f / 255.f, 49.f / 255.f, 0.6f};
    ambientColorAndIntensity = glm::vec4{0.20784314, 0.592156887, 0.56078434, 0.05f};
}

void Game::run(){
    const float FPS = 60.f;
    const float dt = 1.f / FPS;

    auto prevTime = std::chrono::high_resolution_clock::now();
    float accumulator = dt;
    isRunning = true;

    while (isRunning && !glfwWindowShouldClose(window))  {
        // Tính thời gian trôi qua
        const auto newTime = std::chrono::high_resolution_clock::now();
        frameTime = std::chrono::duration<float>(newTime - prevTime).count();
        prevTime = newTime;

        float newFPS = 0.f;
        if (frameTime > 0.0001f) {
            newFPS = 1.f / frameTime;
            if (newFPS > 1000.0f) newFPS = 1000.0f;
        } else {
            newFPS = avgFPS;
        }
        avgFPS = std::lerp(avgFPS, newFPS, 0.1f);

        accumulator += frameTime;
        if (accumulator > 10 * dt) {
            accumulator = dt;
        }

        while (accumulator >= dt) {
            {
                glfwPollEvents();
            }

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            auto timeBeforeInput = std::chrono::high_resolution_clock::now();
            handleInput(dt);
            auto timeAfterInput = std::chrono::high_resolution_clock::now();

            update(dt);
            auto timeAfterUpdate = std::chrono::high_resolution_clock::now();

            accumulator -= dt;
            ImGui::Render();
            auto timeAfterGui = std::chrono::high_resolution_clock::now();

            inputTime = std::chrono::duration<float>(timeAfterInput - timeBeforeInput).count();
            updateTime = std::chrono::duration<float>(timeAfterUpdate - timeAfterInput).count();
            guiTime = std::chrono::duration<float>(timeAfterGui - timeAfterUpdate).count();
        }

        auto timeBeforeDrawList = std::chrono::high_resolution_clock::now();
        generateDrawList();
        auto timeAfterDrawList = std::chrono::high_resolution_clock::now();

        renderer.draw(camera);
        auto timeAfterRender = std::chrono::high_resolution_clock::now();

        drawListTime = std::chrono::duration<float>(timeAfterDrawList - timeBeforeDrawList).count();
        renderTime = std::chrono::duration<float>(timeAfterRender - timeAfterDrawList).count();

        glfwSwapBuffers(window);

        updatePerformanceHistory();

        if (frameLimit) {
            const auto now = std::chrono::high_resolution_clock::now();
            const auto frameTime = std::chrono::duration<float>(now - prevTime).count();
            if (dt > frameTime) {
                std::this_thread::sleep_for(std::chrono::milliseconds(
                    static_cast<std::uint32_t>((dt - frameTime) * 1000)
                ));
            }
        }
    }
}

void Game::handleInput(float dt) {
    cameraController.handleInput(window, camera);
}

void Game::update(float dt) {
    cameraController.update(camera, dt);
    updateEntityTransforms();
    updateDevTools(dt);
}

void Game::updateDevTools(float dt) {
    if (displayFPSDelay > 0.f) {
        displayFPSDelay -= dt;
    } else {
        displayFPSDelay = 1.f;
        displayedFPS = avgFPS;
    }

    ImGui::Begin("Debug");
    ImGui::Text("FPS: %d", (int)displayedFPS);
    ImGui::Text("Frame Time: %.2f ms", frameTime * 1000.0f);

    ImGui::Separator();
    ImGui::Text("Performance Breakdown:");
    ImGui::Text("Input Time: %.2f ms", inputTime * 1000.0f);
    ImGui::Text("Update Time: %.2f ms", updateTime * 1000.0f);
    ImGui::Text("GUI Time: %.2f ms", guiTime * 1000.0f);
    ImGui::Text("Draw List Time: %.2f ms", drawListTime * 1000.0f);
    ImGui::Text("Render Time: %.2f ms", renderTime * 1000.0f);

    ImGui::Separator();
    if (renderTime > 0.016f) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Render time is high!");
    }

    if (ImGui::Checkbox("vSync", &vSync)) {
        // TODO: Cập nhật vSync trong renderer
        // renderer.setVSync(vSync);
    }

    ImGui::Checkbox("Frame limit", &frameLimit);

    const auto cameraPos = camera.getPosition();
    ImGui::Text("Camera pos: %.2f, %.2f, %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
    const auto yaw = cameraController.getYaw();
    const auto pitch = cameraController.getPitch();
    ImGui::Text("Camera rotation: (yaw) %.2f, (pitch) %.2f", yaw, pitch);

    static bool showPerformanceDetails = false;
    ImGui::Checkbox("Show Performance Details", &showPerformanceDetails);

    if (showPerformanceDetails) {
        ImGui::Begin("Performance Details", &showPerformanceDetails);

        // Biểu đồ frame time
        ImGui::Text("Frame Time History");
        ImGui::PlotLines("##frametimes",
                        frameTimeHistory,
                        IM_ARRAYSIZE(frameTimeHistory),
                        frameTimeHistoryIndex,
                        "",
                        0.0f, 33.3f,  // Thang đo từ 0 đến 33.3ms (30 FPS)
                        ImVec2(0, 80));

        // Biểu đồ FPS
        ImGui::Text("FPS History");
        ImGui::PlotLines("##fps",
                        fpsHistory,
                        IM_ARRAYSIZE(fpsHistory),
                        fpsHistoryIndex,
                        "",
                        0.0f, 120.0f,  // Thang đo từ 0 đến 120 FPS
                        ImVec2(0, 80));

        // Hiển thị chi tiết từng phần của frame
        ImGui::Text("Component Time Breakdown");

        // Tạo dữ liệu cho biểu đồ thanh
        float breakdown[] = { inputTime, updateTime, guiTime, drawListTime, renderTime };
        float total = inputTime + updateTime + guiTime + drawListTime + renderTime;

        if (total > 0) {
            // Tạo biểu đồ thanh đơn giản
            ImGui::Text("Input:     %.1f%%", (inputTime/total) * 100.0f);
            ImGui::ProgressBar(inputTime/total, ImVec2(-1, 0), "");

            ImGui::Text("Update:    %.1f%%", (updateTime/total) * 100.0f);
            ImGui::ProgressBar(updateTime/total, ImVec2(-1, 0), "");

            ImGui::Text("GUI:       %.1f%%", (guiTime/total) * 100.0f);
            ImGui::ProgressBar(guiTime/total, ImVec2(-1, 0), "");

            ImGui::Text("Draw List: %.1f%%", (drawListTime/total) * 100.0f);
            ImGui::ProgressBar(drawListTime/total, ImVec2(-1, 0), "");

            ImGui::Text("Render:    %.1f%%", (renderTime/total) * 100.0f);
            ImGui::ProgressBar(renderTime/total, ImVec2(-1, 0), "");
        }

        ImGui::End();
    }


  auto glmToArr = [](const glm::vec4& v) { return std::array<float, 4>{v.x, v.y, v.z, v.w}; };
  auto arrToGLM = [](const std::array<float, 4>& v) { return glm::vec4{v[0], v[1], v[2], v[3]}; };

  auto ambient = glmToArr(ambientColorAndIntensity);
  if (ImGui::ColorEdit3("Ambient", ambient.data())) {
    ambientColorAndIntensity = arrToGLM(ambient);
  }
  ImGui::DragFloat("Ambient intensity", &ambientColorAndIntensity.w, 1.f, 0.f, 1.f);

  auto sunlight = glmToArr(sunlightColorAndIntensity);
  if (ImGui::ColorEdit3("Sunlight", sunlight.data())) {
    sunlightColorAndIntensity = arrToGLM(sunlight);
  }
  ImGui::DragFloat("Sunlight intensity", &sunlightColorAndIntensity.w, 1.f, 0.f, 1.f);

  ImGui::End();

    renderer.updateDevTools(dt);
}

void Game::cleanup() {
    renderer.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Game::createEntitiesFromScene(const Scene& scene) {
    for (const auto& nodePtr : scene.nodes) {
        if (nodePtr) {
            createEntitiesFromNode(scene, *nodePtr);
        }
    }
}

Game::EntityId Game::createEntitiesFromNode(
    const Scene& scene,
    const SceneNode& node,
    EntityId parentId)
{
    auto& e = makeNewEntity();
    e.tag = node.name;

    {
        e.transform = node.transform;
        if (parentId == NULL_ENTITY_ID) {
            e.worldTransform = e.transform.asMatrix();
        } else {
            const auto& parent = entities[parentId];
            e.worldTransform = parent->worldTransform * node.transform.asMatrix();
        }
    }

    {
        e.meshes = scene.meshes[node.meshIndex].primitives;
    }

    {
        e.parentId = parentId;
        for (const auto& childPtr : node.children) {
            if (childPtr) {
                const auto childId = createEntitiesFromNode(scene, *childPtr, e.id);
                e.children.push_back(childId);
            }
        }
    }

    return e.id;
}

Game::Entity& Game::makeNewEntity() {
    entities.push_back(std::make_unique<Entity>());
    auto& e = *entities.back();
    e.id = entities.size() - 1;
    return e;
}

Game::Entity& Game::findEntityByName(std::string_view name) const {
    for (const auto& ePtr : entities) {
        if (ePtr->tag == name) {
            return *ePtr;
        }
    }

    throw std::runtime_error(std::string{"Failed to find entity with name"} + std::string{name});
}

void Game::updateEntityTransforms() {
    const auto I = glm::mat4{1.f};
    for (auto& ePtr : entities) {
        auto& e = *ePtr;
        if (e.parentId == NULL_MESH_ID) {
            updateEntityTransforms(e, I);
        }
    }
}

void Game::updateEntityTransforms(Entity& e, const glm::mat4& parentWorldTransform) {
    const auto prevTransform = e.worldTransform;
    e.worldTransform = parentWorldTransform * e.transform.asMatrix();
    if (e.worldTransform == prevTransform) {
        return;
    }

    for (const auto& childId : e.children) {
        auto& child = *entities[childId];
        updateEntityTransforms(child, e.worldTransform);
    }
}

void Game::generateDrawList() {
  const auto sceneData = GPUSceneData{
    .view = camera.getView(),
    .proj = camera.getProjection(),
    .viewProj = camera.getViewProj(),
    .cameraPos = glm::vec4{camera.getPosition(), 1.f},
    .ambientColorAndIntensity = ambientColorAndIntensity,
    .sunlightDirection = sunlightDir,
    .sunlightColorAndIntensity = sunlightColorAndIntensity,
  };
  renderer.beginDrawing(sceneData);

    for (const auto& ePtr : entities) {
        const auto& e = *ePtr;
        for (const auto& mesh : e.meshes) {
            renderer.addDrawCommand(mesh, e.worldTransform);
        }
    }

    renderer.endDrawing();
}

void Game::updatePerformanceHistory() {
    frameTimeHistory[frameTimeHistoryIndex] = frameTime * 1000.0f;
    fpsHistory[fpsHistoryIndex] = avgFPS;

    frameTimeHistoryIndex = (frameTimeHistoryIndex + 1) % HISTORY_SIZE;
    fpsHistoryIndex = (fpsHistoryIndex + 1) % HISTORY_SIZE;
}
