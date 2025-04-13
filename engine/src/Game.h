#pragma once

#include <memory>
#include <vector>

#include <Graphics/Camera.h>

#include "FreeCameraController.h"
#include "Renderer.h"

class GLFWwindow;

class Game {
    public:
        static const std::size_t NULL_ENTITY_ID = std::numeric_limits<std::size_t>::max();

        using EntityId = std::size_t;

        struct Entity {
            EntityId id{NULL_ENTITY_ID};
            std::string tag;

            Transform transform;
            glm::mat4 worldTransform{1.f};

            EntityId parentId{NULL_ENTITY_ID};
            std::vector<EntityId> children;

            std::vector<MeshId> meshes;


        };

    public:
        void init();
        void run();
        void cleanup();

    private:
        void handleInput(float dt);
        void update(float dt);
        void updateDevTools(float dt);

        void createEntitiesFromScene(const Scene& scene);
        EntityId createEntitiesFromNode(
            const Scene& scene,
            const SceneNode& node,
            EntityId parentId = NULL_ENTITY_ID
        );

        std::vector<std::unique_ptr<Entity>> entities;
        Entity& makeNewEntity();
        Entity& findEntityByName(std::string_view name) const;

        void updateEntityTransforms();
        void updateEntityTransforms(Entity& e, const glm::mat4& parentWorldTransform);

        void generateDrawList();
        void sortDrawList();

        Renderer renderer;

        GLFWwindow* window;

        bool isRunning{false};
        bool vSync{true};
        bool frameLimit{true};
        float frameTime{0.f};
        float avgFPS{0.f};

        float displayedFPS{0.f};
        float displayFPSDelay{1.f};

        float inputTime = 0.f;
        float updateTime = 0.f;
        float renderTime = 0.f;
        float drawListTime = 0.f;
        float guiTime = 0.f;

        static const int HISTORY_SIZE = 100;
        float frameTimeHistory[HISTORY_SIZE] = {0};
        float fpsHistory[HISTORY_SIZE] = {0};
        int frameTimeHistoryIndex = 0;
        int fpsHistoryIndex = 0;

        void updatePerformanceHistory();

        Camera camera;
        FreeCameraController cameraController;
};
