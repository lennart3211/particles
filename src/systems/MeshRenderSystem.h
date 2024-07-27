#pragma once

#include "Buffer.h"
#include "Device.h"
#include "FrameInfo.h"
#include "Mesh.h"
#include "Pipeline.h"
#include "Rectangle.h"

#include <memory>

namespace engine {

class MeshRenderSystem {
private:
  Device &m_device;
  std::unique_ptr<Pipeline> m_pipeline;
  VkPipelineLayout m_pipelineLayout;

public:
  MeshRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, const std::string &vertPath, const std::string &fragPath);

  ~MeshRenderSystem();

  MeshRenderSystem(const MeshRenderSystem &) = delete;

  MeshRenderSystem &operator=(const MeshRenderSystem &) = delete;

  void Render(FrameInfo &frameInfo, std::vector<std::unique_ptr<Mesh>> &meshes);

private:
  void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void CreatePipeline(VkRenderPass renderPass, const std::string &vertPath,
                      const std::string &fragPath);
};

} // engine
