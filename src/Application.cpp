#include "Application.h"

#include "Imgui.h"
#include "systems/ParticleRenderSystem.h"
#include <descriptors/DescriptorWriter.h>

#include <GLFW/glfw3.h>

#include <chrono>
#include <memory>
#include <random>

namespace engine {

    Application::Application() {
        mGlobalPool = DescriptorPool::Builder(mDevice)
                .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, SwapChain::MAX_FRAMES_IN_FLIGHT)
                .build();
      }

    Application::~Application() = default;

    void Application::run() {
        Imgui imgui{mWindow, mDevice, mRenderer.GetSwapChainRenderPass(), mRenderer.GetImageCount()};

        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("../font/MontserratAlternates-Bold.otf", 32.0f);

        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (auto &uboBuffer : uboBuffers) {
            uboBuffer = std::make_unique<Buffer>(
                    mDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffer->map();
        }

        auto globalSetLayout = DescriptorSetLayout::Builder(mDevice)
                .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                            VK_SHADER_STAGE_ALL_GRAPHICS)
                .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *mGlobalPool)
                    .writeBuffer(0, &bufferInfo)
                    .build(globalDescriptorSets[i]);
        }

        m_backgroundColor = glm::vec3(0.3f, 0.5f, 1.0f);
        mRenderer.SetClearColor(m_backgroundColor);

        auto currentTime = std::chrono::high_resolution_clock::now();

        while (!mWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            if (auto commandBuffer = mRenderer.BeginFrame()) {
                imgui.newFrame();

                int frameIndex = (int) mRenderer.GetFrameIndex();

                FrameInfo frameInfo{
                        frameIndex,
                        frameTime,
                        commandBuffer,
                        {globalDescriptorSets[frameIndex]}
                      };

                mRenderer.SetClearColor(m_backgroundColor);

                GlobalUbo ubo{};

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();



                // render
                mRenderer.BeginSwapChainRenderPass(commandBuffer);

                ImGui::Begin("Settings");
                {
                    ImGui::Text("Time since last frame: %f", frameTime);

                    ImGui::ColorPicker3("Background Color", &m_backgroundColor.x);

                }
                ImGui::End();

                imgui.render(commandBuffer);

                mRenderer.EndSwapChainRenderPass(commandBuffer);
                mRenderer.EndFrame();
            }
        }
    }

    void Application::Update() {}

    void Application::Render() {}

    float Application::frand(float min, float max) {
        static std::mt19937 generator(static_cast<unsigned int>(std::time(nullptr)));
        std::uniform_real_distribution<float> distribution(min, max);
        return distribution(generator);
    }

} // namespace engine
