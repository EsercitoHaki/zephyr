#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <vector>

#include <vulkan/vulkan.h>

#include <DeletionQueue.h>
#include <VkDescriptors.h>
#include <VkTypes.h>
#include <VkBootstrap.h>
#include <vk_mem_alloc.h>

#include <glm/vec4.hpp>

#include <Graphics/Mesh.h>
#include <Graphics/Scene.h>

#include "DrawCommand.h"
#include "MaterialCache.h"
#include "MeshCache.h"

struct Scene;
struct SceneNode;

class Camera;

class GLFWwindow;

class Renderer {
public:
    static constexpr std::size_t FRAME_OVERLAP = 2;

    template<typename T>
        struct AppendableBuffer {
        void append(std::span<const T> elements)
        {
            assert(size + elements.size() <= capacity);
            auto arr = (T*)buffer.info.pMappedData;
            memcpy((void*)&arr[size], elements.data(), elements.size() * sizeof(glm::mat4));
            size += elements.size();
        }

        void clear() { size = 0; }

        VkBuffer getVkBuffer() const { return buffer.buffer; }

        AllocatedBuffer buffer;
        std::size_t capacity{};
        std::size_t size{0};
    };

    struct FrameData {
        VkCommandPool commandPool;
        VkCommandBuffer mainCommandBuffer;
        VkSemaphore swapchainSemaphore;
        VkSemaphore renderSemaphore;
        VkFence renderFence;
        DeletionQueue deletionQueue;

        DescriptorAllocatorGrowable frameDescriptors;
        AppendableBuffer<glm::mat4> jointMatricesBuffer;
        VkDeviceAddress jointMatricesBufferAddress;
    };

public:
    void init(GLFWwindow* window, bool vSync);
    void draw(const Camera& camera);
    void cleanup();

    void uploadMesh(const Mesh& cpuMesh, GPUMesh& mesh) const;
    SkinnedMesh createSkinnedMeshBuffer(MeshId meshId) const;

    [[nodiscard]] AllocatedBuffer createBuffer(
        std::size_t allocSize,
        VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage) const;

    [[nodiscard]] VkDeviceAddress getBufferAddress(const AllocatedBuffer& buffer) const;

    [[nodiscard]] AllocatedImage createImage(
        void* data,
        VkExtent3D size,
        VkFormat format,
        VkImageUsageFlags usage,
        bool mipMap
    );

    [[nodiscard]] AllocatedImage loadImageFromFile(
        const std::filesystem::path& path,
        VkFormat format,
        VkImageUsageFlags usage,
        bool mipMap
    );

    void addDebugLabel(const AllocatedImage& image, const char* label);
    void addDebugLabel(const VkShaderModule& shader, const char* label);
    void addDebugLabel(const VkPipeline& pipeline, const char* label);
    void addDebugLabel(const AllocatedBuffer& buffer, const char* label);

    void beginCmdLabel(VkCommandBuffer cmd, const char* label);
    void endCmdLabel(VkCommandBuffer cmd);

    void destroyBuffer(const AllocatedBuffer& buffer) const;

    void destroyImage(const AllocatedImage& image) const;
    VkDescriptorSet writeMaterialData(MaterialId id, const Material& material);


    void updateDevTools(float dt);

    void beginDrawing(const GPUSceneData& sceneData);
    void addDrawCommand(MeshId id, const glm::mat4& transform);
    void addDrawSkinnedMeshCommand(
        std::span<const MeshId> meshes,
        std::span<const SkinnedMesh> skinnedMeshes,
        const glm::mat4& transform,
        std::span<const glm::mat4> jointMatrices);
    void endDrawing();

    Scene loadScene(const std::filesystem::path& path);
private:
    void initVulkan(GLFWwindow* window);
    void loadExtensionFunctions();
    void createSwapchain(std::uint32_t width, std::uint32_t height, bool vSync);
    void createCommandBuffers();
    void initSyncStructures();
    void initImmediateStructures();
    void initDescriptorAllocators();
    void initDescriptors();
    void initSamplers();
    void initDefaultTextures();

    void allocateMaterialDataBuffer(std::size_t numMaterials);

    void initPipelines();
    void initBackgroundPipelines();
    void initSkinningPipeline();
    void initTrianglePipeline();
    void initMeshPipeline();

    void initImGui(GLFWwindow* window);

    void destroyCommandBuffers();
    void destroySyncStructures();

    AllocatedImage createImage(
        VkExtent3D size,
        VkFormat format,
        VkImageUsageFlags usage,
        bool mipMap
    );

    FrameData& getCurrentFrame();
    void doSkinning(VkCommandBuffer cmd);
    void drawBackground(VkCommandBuffer cmd);
    void drawGeometry(VkCommandBuffer cmd, const Camera& camera);
    VkDescriptorSet uploadSceneData();
    void drawImGui(VkCommandBuffer cmd, VkImageView targetImageView);

    void sortDrawList();

    void immediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function) const;

private: //data
    vkb::Instance instance;
    vkb::PhysicalDevice physicalDevice;
    vkb::Device device;
    VmaAllocator allocator;
    VkQueue graphicsQueue;
    std::uint32_t graphicsQueueFamily;

    VkSurfaceKHR surface;
    vkb::Swapchain swapchain;
    std::vector<VkImage> swapchainImages;
    std::vector<VkImageView> swapchainImageViews;
    VkExtent2D swapchainExtent;

    DeletionQueue deletionQueue;
    DescriptorAllocatorGrowable descriptorAllocator;

    AllocatedImage drawImage;
    AllocatedImage depthImage;
    VkExtent2D drawExtent;

    VkDescriptorSetLayout drawImageDescriptorLayout;
    VkDescriptorSet drawImageDescriptors;

    std::array<FrameData, FRAME_OVERLAP> frames{};
    std::uint32_t frameNumber{0};

    VkFence immFence;
    VkCommandBuffer immCommandBuffer;
    VkCommandPool immCommandPool;

    VkPipelineLayout gradientPipelineLayout;
    VkPipeline gradientPipeline;
    struct ComputePushConstants {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };
    ComputePushConstants gradientConstants;

    VkPipelineLayout skinningPipelineLayout;
    VkPipeline skinningPipeline;
    struct SkinningPushConstants {
            VkDeviceAddress jointMatricesBuffer;
            std::uint32_t jointMatricesStartIndex;
            std::uint32_t numVertices;
            VkDeviceAddress inputBuffer;
            VkDeviceAddress skinningData;
            VkDeviceAddress outputBuffer;
        };

    static constexpr std::size_t MAX_JOINT_MATRICES = 5000;

    VkPipelineLayout trianglePipelineLayout;
    VkPipeline trianglePipeline;

    VkPipelineLayout meshPipelineLayout;
    VkPipeline meshPipeline;
    VkDescriptorSetLayout sceneDataDescriptorLayout;

    VkDescriptorSetLayout meshMaterialLayout;
    GPUSceneData sceneData;

    MaterialCache materialCache;
    AllocatedBuffer materialDataBuffer;

    MeshCache meshCache;

    Scene scene;

    VkSampler defaultSamplerNearest;

    AllocatedImage whiteTexture;

    std::vector<DrawCommand> drawCommands;
    std::vector<std::size_t> sortedDrawCommands;

    PFN_vkSetDebugUtilsObjectNameEXT pfnSetDebugUtilsObjectNameEXT;
    PFN_vkQueueBeginDebugUtilsLabelEXT pfnQueueBeginDebugUtilsLabelEXT;
    PFN_vkQueueEndDebugUtilsLabelEXT pfnQueueEndDebugUtilsLabelEXT;
    PFN_vkCmdBeginDebugUtilsLabelEXT pfnCmdBeginDebugUtilsLabelEXT;
    PFN_vkCmdEndDebugUtilsLabelEXT pfnCmdEndDebugUtilsLabelEXT;
    PFN_vkCmdInsertDebugUtilsLabelEXT pfnCmdInsertDebugUtilsLabelEXT;
};
