#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FROCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>

namespace engine {
    class  Camera {
    private:
        glm::mat4 m_ProjectionMatrix{1.0f};
        glm::mat4 m_ViewMatrix{1.0f};

        glm::vec3 m_position{0};
        glm::vec3 m_direction{1, 0, 0};
        glm::vec3 m_rotation{0};
        glm::vec3 m_up{0, -1, 0};

    public:
        void SetPosition(const glm::vec3 &position);

        void SetDirection(const glm::vec3 &direction);

        void SetRotation(const glm::vec3 &rotation);

        void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);

        void SetPerspectiveProjection(float fovy, float ascpect, float near, float far);

        void SetViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3(0, -1, 0));

        void SetViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3(0, -1, 0));

        void SetViewYXZ(glm::vec3 position, glm::vec3 rotation);

        [[nodiscard]] const glm::mat4 &Projection() const { return m_ProjectionMatrix; }
        [[nodiscard]] const glm::mat4 &View() const { return m_ViewMatrix; }
        [[nodiscard]] const glm::vec3 &Position() const { return m_position; }
        [[nodiscard]] const glm::vec3 &Direction() const { return m_direction; }
        [[nodiscard]] const glm::vec3 &Rotation() const { return m_rotation; }
    };

} // namespace engine