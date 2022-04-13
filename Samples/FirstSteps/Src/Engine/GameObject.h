#pragma once

#include "Utilities/Model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace Divide {
    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{ 1.f, 1.f, 1.f };
        glm::vec3 rotation{};
        [[nodiscard]] glm::mat4 mat4() const;
        [[nodiscard]] glm::mat3 normalMatrix() const;
    };

    class GameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;

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
        TransformComponent _transform{};

    private:
        GameObject(const id_t objId) : _id(objId) {}

    private:
        id_t _id = 0u;
    };
}; //namespace Divide
