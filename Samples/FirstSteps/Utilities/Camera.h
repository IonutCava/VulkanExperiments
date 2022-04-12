#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Divide {
    class Camera {
    public:
        void setOrthographicProjection(float left, float right, float top, float botton, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        [[nodiscard]] inline const glm::mat4& getProjection() const { return _projectionMatrix; }
    private:

        glm::mat4 _projectionMatrix{ 1.f };

    };
}; //namespace Divide
