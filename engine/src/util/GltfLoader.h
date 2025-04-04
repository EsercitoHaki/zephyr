#pragma once

#include <filesystem>
#include <unordered_map>

#include <Graphics/GPUMesh.h>
#include <Graphics/Material.h>
#include <Graphics/Mesh.h>
#include <Math/Transform.h>

struct Model;
struct Scene;

class MaterialCache;
class MeshCache;
class Renderer;

namespace util
{
struct LoadContext {
    Renderer& renderer;
    MaterialCache& materialCache;
    MeshCache& meshCache;
    AllocatedImage& whiteTexture;
};

class SceneLoader {
public:
    void loadScene(const LoadContext& context, Scene& scene, const std::filesystem::path& path);

private:
    std::unordered_map<std::size_t, MaterialId> materialMapping;

    std::unordered_map<int, JointId> gltfNodeIdxToJointId;
};

}