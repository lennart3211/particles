#include "Application.h"

#include "Imgui.h"
#include "systems/ParticleRenderSystem.h"
#include "Camera.h"
#include "systems/ParticleSystem.h"
#include <descriptors/DescriptorWriter.h>

#include <GLFW/glfw3.h>

#include <chrono>
#include <memory>
#include <random>
#include <iostream>

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

        Camera camera;
        camera.SetDirection(glm::vec3(-1, -0.05, 0));
        camera.SetPosition(glm::vec3(3.5, 0, 0));

        ParticleSystem particleSystem{mDevice,
                                      mRenderer.GetSwapChainRenderPass(),
                                      globalSetLayout->getDescriptorSetLayout(),
                                      50,
                                      2};

        particleSystem.AddParticle(glm::vec3(0, 0, 0), glm::vec4(1, 0.5, 1, 1), 0.05, glm::vec3(0, 0, 0), 1.0f, true);

        particleSystem.AddBox(glm::vec3(0), glm::vec3(0.5));

        m_backgroundColor = glm::vec3(0.3f, 0.5f, 1.0f);
        mRenderer.SetClearColor(m_backgroundColor);

        auto currentTime = std::chrono::high_resolution_clock::now();

        glm::vec3 p_position{0};
        glm::vec3 p_velocity{frand(-0.5, 0.5), frand(-0.5, 0.5), frand(-0.5, 0.5)};
        glm::vec4 p_color{frand(0, 1), frand(0, 1), frand(0, 1), 1};
        float p_radius{0.05};
        float p_mass{1};
        bool p_static{false};

        while (!mWindow.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            float aspect = mRenderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

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
                ubo.view = camera.View();
                ubo.projection = camera.Projection();
                ubo.ambientLightColor = glm::vec4(m_backgroundColor, 1);

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                particleSystem.Update(frameTime);

                // render
                mRenderer.BeginSwapChainRenderPass(commandBuffer);

                ImGui::Begin("Settings");
                {
                    ImGui::Text("Time since last frame: %f", frameTime);

                    ImGui::ColorPicker3("Background Color", &m_backgroundColor.x);

                    glm::vec3 cameraPos = camera.Position();
                    ImGui::SliderFloat3("Camera Position", &cameraPos.x, -50, 50);
                    camera.SetPosition(cameraPos);
                    glm::vec3 cameraDir = camera.Direction();
                    ImGui::SliderFloat3("Camera Direction", &cameraDir.x, -1, 1);
                    camera.SetDirection(cameraDir);

                }
                ImGui::End();

                ImGui::Begin("Particles");
                {
                    if (ImGui::Button("Add Particle")) {
                        particleSystem.AddParticle(p_position, p_color, p_radius, p_velocity, p_mass, p_static);
                        p_velocity = {frand(-0.5, 0.5), frand(-0.5, 0.5), frand(-0.5, 0.5)};
                        p_color = {frand(0, 1), frand(0, 1), frand(0, 1), 1};
                    }

                    auto pair = particleSystem.Particles();
                    for (uint32_t i = 0; i < pair.first.size(); ++i) {
                        ImGui::PushID((int32_t) i);
                        ImGui::DragFloat3("Position", &pair.first[i].position.x, 0.05);
                        ImGui::DragFloat3("Velocity", &pair.second[i].velocity.x, 0.05);
                        ImGui::DragFloat("Size", &pair.first[i].radius, 0.05);
                        ImGui::DragFloat("Mass", &pair.second[i].mass, 0.05);
                        ImGui::Checkbox("Static", &pair.second[i].isStatic);
                        ImGui::PopID();
                    }
                }
                ImGui::End();

                ImGui::Begin("Boxes");
                {
                    auto &boxes = particleSystem.Boxes();
                    for (uint32_t i = 0; i < boxes.size(); ++i) {
                        ImGui::PushID((int32_t) i);
                        ImGui::DragFloat3("Position", &boxes[i].position.x, 0.05);
                        ImGui::DragFloat3("Half Extent", &boxes[i].halfExtent.x, 0.05);
                        ImGui::PopID();
                    }
                }
                ImGui::End();

                particleSystem.Render(frameInfo);

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
