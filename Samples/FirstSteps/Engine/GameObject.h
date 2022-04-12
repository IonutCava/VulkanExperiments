#pragma once

#include "../Utilities/Model.h"

#include <memory>

namespace Divide {
    struct Transform2DComponent {
        glm::vec2 translation{};
        glm::vec2 scale{ 1.f, 1.f };
        float rotation = 0.f;

        [[nodiscard]] inline glm::mat2 mat2() const { 
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            const glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
            const glm::mat2 rotMat{ {c, s}, {-s, c} };
            return rotMat * scaleMat;
        }
    };

    class GameObject {
    public:
        using id_t = unsigned int;

        static GameObject CreateGameObject() {
            static id_t currentId = 0u;
            return GameObject{ currentId++ };
        }

        //GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        //GameObject(GameObject&&) = delete;
        //GameObject& operator=(GameObject&&) = delete;

        [[nodiscard]] inline id_t getId() const { return _id; }

        std::shared_ptr<Model> _model{};
        glm::vec3 _colour{};
        Transform2DComponent _transform2D{};

    private:
        GameObject(const id_t objId) : _id(objId) {}

    private:
        id_t _id = 0u;
    };
}; //namespace Divide
