#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <string>

namespace Divide {
    class Window {
    public:
        Window() = default;
        Window(int w, int h, const char* name);
        ~Window();

        Window(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(const Window&) = delete;
        Window&& operator=(Window&&) = delete;

        [[nodiscard]] inline bool shouldClose() const { return _handle == nullptr || glfwWindowShouldClose(_handle); }
        [[nodiscard]] inline VkExtent2D getExtent() const { return { static_cast<uint32_t>(_width), static_cast<uint32_t>(_height)}; }
        [[nodiscard]] inline bool wasWindowResized() const { return _framebufferResized; }
        void resetWindowResizedFlag() { _framebufferResized = false; }
        void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

    private:
        static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
        void initWindow();

        int _width = 0;
        int _height = 0;
        bool _framebufferResized = false;

        std::string _name = "";
        GLFWwindow* _handle = nullptr;
    };

}; //namespace Divide