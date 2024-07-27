#pragma once


#include <vulkan/vulkan.h>

#include <glm/glm.hpp>

namespace engine {


    struct PointLight {
        glm::vec4 position{};
        glm::vec4 color{};
    };

    struct GlobalUbo {
        glm::mat4 projection{1.0f};
        glm::mat4 view{1.0f};
        glm::vec4 ambientLightColor{0.3f, 0.3f, 1.0f, .02f};
    };

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        std::vector<VkDescriptorSet> descriptorSets;
    };
}

