#pragma once

#include "Buffer.h"
#include "Device.h"
#include "FrameInfo.h"
#include "Model.h"
#include "Particle.h"
#include "Pipeline.h"

#include <memory>

namespace engine {

    class ParticleRenderSystem {
    private:
        Device &m_device;
        std::unique_ptr<Pipeline> m_pipeline;
        VkPipelineLayout m_pipelineLayout;

        std::vector<Particle> m_particles;
        const uint32_t m_maxParticles;

        std::unique_ptr<Buffer> m_vertexBuffer;
        uint32_t m_vertexCount;

        std::unique_ptr<Buffer> m_instanceBuffer;

        std::unique_ptr<Buffer> m_indexBuffer;
        uint32_t m_indexCount;

    public:
        ParticleRenderSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, uint32_t maxParticles);

        ~ParticleRenderSystem();

        ParticleRenderSystem(const ParticleRenderSystem &) = delete;

        ParticleRenderSystem &operator=(const ParticleRenderSystem &) = delete;

        void Render(FrameInfo &frameInfo);

        Particle *AddParticle(const glm::vec3 &position, const glm::vec4 &color, float size);
        void RemoveParticle(const Particle *particle);

        void SetParticleMesh(const std::string &filepath);

    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void CreatePipeline(VkRenderPass renderPass);

        void CreateVertexBuffer(const std::vector<Model::Vertex> &vertices);
        void CreateInstanceBuffer();
        void CreateIndexBuffer(const std::vector<uint32_t> &indices);

        void UpdateInstanceBuffer();

        void Bind(VkCommandBuffer commandBuffer);
    };

} // engine
