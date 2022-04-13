#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Divide {
    class Camera {
    public:
        void setOrthographicProjection(float left, float right, float top, float botton, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
        void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

        [[nodiscard]] inline const glm::mat4& getProjection() const { return _projectionMatrix; }
        [[nodiscard]] inline const glm::mat4& getView() const { return _viewMatrix; }
        [[nodiscard]] inline const glm::mat4& getInverseView() const { return _inverseViewMatrix; }

    private:
        glm::mat4 _projectionMatrix{ 1.f };
        glm::mat4 _viewMatrix{ 1.f };
        glm::mat4 _inverseViewMatrix{ 1.f };
    };
}; //namespace Divide
