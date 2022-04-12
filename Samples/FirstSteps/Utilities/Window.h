#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <string>

namespace Divide {
    class Window {
    public:
        Window(int w, int h, const char* name);
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window&& operator=(Window&&) = delete;

        [[nodiscard]] inline bool shouldClose() { return _handle == nullptr || glfwWindowShouldClose(_handle); }
        [[nodiscard]] inline VkExtent2D getExtent() { return { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height)}; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    private:
        void initWindow();

        const int _width = 0;
        const int _height = 0;

        std::string _name = "";
        GLFWwindow* _handle = nullptr;
    };

}; //namespace Divide