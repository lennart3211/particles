#pragma once

#include "Device.h"
#include "Pipeline.h"
#include "FrameInfo.h"
#include "Buffer.h"
#include "Rectangle.h"

#include <memory>

namespace engine {

class RectangleRenderSystem {
private:
  Device &m_device;
  std::unique_ptr<Pipeline> m_pipeline;
  VkPipelineLayout m_pipelineLayout;

  std::vector<Rectangle> m_rectangles;
  const uint32_t m_maxRectangles;

  std::unique_ptr<Buffer> m_vertexBuffer;

public:
  RectangleRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, uint32_t maxRectangles);

  ~RectangleRenderSystem();

  RectangleRenderSystem(const RectangleRenderSystem &) = delete;

  RectangleRenderSystem &operator=(const RectangleRenderSystem &) = delete;

  void Render(FrameInfo &frameInfo);

  Rectangle *AddRectangle(const glm::vec2 &position, const glm::vec4 &color,
                          const glm::vec2 &extent);

  void RemoveRectangle(const Rectangle *rectangle);

private:
  void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
  void CreatePipeline(VkRenderPass renderPass);

  void CreateVertexBuffer();
  void UpdateVertexBuffer();

  void Bind(VkCommandBuffer commandBuffer);
};

} // engine
