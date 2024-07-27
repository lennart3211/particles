//
// Created by lennart on 7/5/24.
//

#include "Particle.h"
#include "Model.h"

namespace engine {
    std::vector<VkVertexInputBindingDescription> Particle::getBindingDescription() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(2);

        bindingDescriptions[0] = Model::Vertex::getBindingsDescriptions()[0];

        bindingDescriptions[1].binding = 1;
        bindingDescriptions[1].stride = sizeof(Particle);
        bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Particle::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        const std::vector<VkVertexInputAttributeDescription> &vertexAttributeDescriptions = Model::Vertex::getAttributeDescriptions();
        for (auto &item : vertexAttributeDescriptions) {
          attributeDescriptions.push_back(item);
        }

        attributeDescriptions.push_back({5, 1, VK_FORMAT_R32G32B32_SFLOAT, (uint32_t) offsetof(Particle, position)});
        attributeDescriptions.push_back({6, 1, VK_FORMAT_R32G32B32A32_SFLOAT, (uint32_t) offsetof(Particle, color)});
        attributeDescriptions.push_back({7, 1, VK_FORMAT_R32_SFLOAT, (uint32_t) offsetof(Particle, size)});

        return attributeDescriptions;
    }

    glm::vec3 LinkedParticle::distToChild() const {
        return particle->position - child->position;
    }
} // engine