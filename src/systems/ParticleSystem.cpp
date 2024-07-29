//
// Created by lennart on 7/26/24.
//

#include <iostream>
#include "ParticleSystem.h"

namespace engine {
    void ParticleSystem::AddParticle(const glm::vec3 &position, const glm::vec4 &color, float radius,
                                     const glm::vec3 &velocity,
                                     float mass, bool isStatic) {
        m_particles.emplace_back(position, color, radius);
        m_physicsParticles.emplace_back(m_particles.size() - 1, velocity, mass, isStatic);
        m_renderSystem.UpdateInstanceBuffer(m_particles);
    }

    void ParticleSystem::RemoveParticle(uint32_t i) {
        if (i >= m_particles.size()) return;

        assert(m_particles.size() == m_physicsParticles.size() && "m_particles and m_physicsParticles need to be the same size");

        std::swap(m_particles[i], m_particles.back());
        std::swap(m_physicsParticles[i], m_physicsParticles.back());

        if (i < m_particles.size() - 1) {
            m_physicsParticles[i].particleIndex = i;
        }

        m_particles.pop_back();
        m_physicsParticles.pop_back();

        m_renderSystem.UpdateInstanceBuffer(m_particles);
    }

    void ParticleSystem::Render(FrameInfo &frameInfo) {
        m_renderSystem.Render(frameInfo, m_particles.size());
        m_boxRenderSystem.Render(frameInfo, m_boxes.size());
    }

    void ParticleSystem::Update(float dt) {
        CheckCollisions();
        CheckBoxCollisions();

        for (auto &physicsParticle : m_physicsParticles) {
            if (!physicsParticle.isStatic) physicsParticle.velocity += glm::vec3(0, 9.81, 0) * dt;
            m_particles[physicsParticle.particleIndex].position += physicsParticle.velocity * dt;
        }
        m_renderSystem.UpdateInstanceBuffer(m_particles);
        m_boxRenderSystem.UpdateVertexBuffer(m_boxes);
    }

    void ParticleSystem::CheckCollisions() {
        for (size_t i = 0; i < m_physicsParticles.size(); ++i) {
            for (size_t j = i + 1; j < m_physicsParticles.size(); ++j) {
                auto& p1 = m_physicsParticles[i];
                auto& p2 = m_physicsParticles[j];

                glm::vec3 diff = m_particles[p1.particleIndex].position - m_particles[p2.particleIndex].position;
                float distance = glm::length(diff);
                float minDist = m_particles[p1.particleIndex].radius + m_particles[p2.particleIndex].radius;

                if (distance < minDist) {
                    HandleCollision(p1, p2);
                }
            }
        }
    }

    void ParticleSystem::HandleCollision(PhysicsParticle& p1, PhysicsParticle& p2) {
        glm::vec3 normal = glm::normalize(m_particles[p2.particleIndex].position - m_particles[p1.particleIndex].position);

        // Relative velocity
        glm::vec3 rv = p2.velocity - p1.velocity;

        // Calculate relative velocity in terms of the normal direction
        float velAlongNormal = glm::dot(rv, normal);

        // Do not resolve if velocities are separating
        if (velAlongNormal > 0)
            return;

        // Calculate restitution (you might want to store this as a property of the particle)
        float e = 0.8f;

        // Calculate impulse scalar
        float j = -(1 + e) * velAlongNormal;
        j /= 1 / p1.mass + 1 / p2.mass;

        // Apply impulse
        glm::vec3 impulse = j * normal;
        p1.velocity -= 1 / p1.mass * impulse;
        p2.velocity += 1 / p2.mass * impulse;
    }

    std::pair<std::vector<Particle>&, std::vector<PhysicsParticle>&> ParticleSystem::Particles() {
        return {m_particles, m_physicsParticles};
    }

    void ParticleSystem::AddBox(const glm::vec3 &position, const glm::vec3 &halfExtent) {
        m_boxes.emplace_back(position, halfExtent);
        m_boxRenderSystem.UpdateVertexBuffer(m_boxes);
    }

    void ParticleSystem::CheckBoxCollisions() {
        for (auto& physicsParticle : m_physicsParticles) {
            glm::vec3& pos = m_particles[physicsParticle.particleIndex].position;
            float radius = m_particles[physicsParticle.particleIndex].radius;

            for (const auto& box : m_boxes) {
                glm::vec3 relativePos = pos - box.position;
                glm::vec3 penetration;
                bool collision = false;

                for (int i = 0; i < 3; ++i) {
                    if (std::abs(relativePos[i]) > box.halfExtent[i] - radius) {
                        float sign = glm::sign(relativePos[i]);
                        penetration[i] = (box.halfExtent[i] - radius) * sign - relativePos[i];
                        collision = true;
                    } else {
                        penetration[i] = 0;
                    }
                }

                if (collision) {
                    // Determine which face was hit (largest penetration)
                    int hitAxis = 0;
                    for (int i = 1; i < 3; ++i) {
                        if (std::abs(penetration[i]) > std::abs(penetration[hitAxis])) {
                            hitAxis = i;
                        }
                    }

                    // Adjust position
                    pos[hitAxis] += penetration[hitAxis];

                    // Reflect velocity along the hit axis
                    physicsParticle.velocity[hitAxis] *= -0.8f; // Apply some energy loss
                }
            }
        }
    }
} // engine