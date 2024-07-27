//
// Created by lennart on 7/5/24.
//

#ifndef ENGINE_PARTICLE_H
#define ENGINE_PARTICLE_H

#include "glm/vec3.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>
#include <vulkan/vulkan.h>

namespace engine {

    struct Particle {
        glm::vec3 position;
        glm::vec4 color;
        float size;

        Particle()
                : position(glm::vec3(0)), size(0), color(glm::vec4(1)) {}

        Particle(const glm::vec3 &position, const float size)
            : position(position), size(size), color(glm::vec4(1)) {}

        Particle(const glm::vec3 &position, const glm::vec4 &color, const float size)
                : position(position), size(size), color(color) {}

        static std::vector<VkVertexInputBindingDescription> getBindingDescription();
        static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    struct LinkedParticle {
        Particle *particle;
        Particle *child;

        LinkedParticle(Particle *particle, Particle *child) : particle(particle), child(child) {}

        glm::vec3 distToChild() const;
    };

    struct Apple {
        Particle *particle;
    };

} // engine

#endif //ENGINE_PARTICLE_H
