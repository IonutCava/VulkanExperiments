#pragma once

#include "Utilities/Window.h"
#include "Utilities/Device.h"
#include "Utilities/Model.h"
#include "Engine/GameObject.h"
#include "Engine/Renderer.h"
#include "Utilities/Descriptors.h"

#include <memory>

namespace Divide {
    class Application {
    public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        Application();
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void run();

    private:
        void loadGameObjects();

        Window _window{WIDTH, HEIGHT, "Hiya Vulkan"};
        Device _device{_window};
        Renderer _renderer{ _window, _device };

        std::unique_ptr<DescriptorPool> _globalPoolPtr{};
        GameObject::Map _gameObjects;
    };
}; //namespace Divide