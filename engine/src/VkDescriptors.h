#pragma once

#include <cstdint>
#include <span>
#include <deque>
#include <vector>

#include <vulkan/vulkan.h>

class DescriptorLayoutBuilder {
public:
  void addBinding(std::uint32_t binding, VkDescriptorType type);
  void clear();
  VkDescriptorSetLayout build(VkDevice device, VkShaderStageFlags shaderStages);

private:
  std::vector<VkDescriptorSetLayoutBinding> bindings;
};

class DescriptorAllocator {
public:
  struct PoolSizeRatio {
    VkDescriptorType type;
    float ratio;
  };

public:
  void initPool(VkDevice device, std::uint32_t maxSets,
                std::span<const PoolSizeRatio> poolRatios);
  void clearDescriptors(VkDevice device);
  void destroyPool(VkDevice device);

  VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);

private:
  VkDescriptorPool pool;
};

class DescriptorAllocatorGrowable {
public:
  struct PoolSizeRatio {
    VkDescriptorType type;
    float ratio;
  };

  void init(VkDevice device, std::uint32_t initialSets,
            std::span<const PoolSizeRatio> poolRatios);
  void clearPools(VkDevice device);
  void destroyPools(VkDevice device);

  VkDescriptorSet allocate(VkDevice device, VkDescriptorSetLayout layout);

private:
  VkDescriptorPool getPool(VkDevice device);
  VkDescriptorPool createPool(VkDevice device, std::uint32_t setCount,
                              std::span<const PoolSizeRatio> poolRatios);

  std::vector<PoolSizeRatio> ratios;
  std::vector<VkDescriptorPool> fullPools;
  std::vector<VkDescriptorPool> readyPools;
  std::uint32_t setsPerPool;
};

class DescriptorWriter {
public:
  void writeBuffer(std::uint32_t binding, VkBuffer buffer, size_t size,
                   size_t offset, VkDescriptorType type);
  void writeImage(std::uint32_t binding, VkImageView image, VkSampler sampler,
                  VkImageLayout layout, VkDescriptorType type);

  void clear();
  void updateSet(VkDevice device, VkDescriptorSet set);

private:
  std::deque<VkDescriptorImageInfo> imageInfos;
  std::deque<VkDescriptorBufferInfo> bufferInfos;
  std::vector<VkWriteDescriptorSet> writes;
};