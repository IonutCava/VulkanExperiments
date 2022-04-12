#include "Window.h"

#include <stdexcept>

namespace Divide {
    Window::Window(const int w, const int h, const char* name)
        : _width{ w }
        , _height{ h }
        , _name{ name }
    {
        initWindow();
    }

    Window::~Window()
    {
        glfwDestroyWindow(_handle);
        glfwTerminate();
    }

    void Window::initWindow()
    {
        if (_handle != nullptr) {
            glfwDestroyWindow(_handle);
            glfwTerminate();
        }

        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        _handle = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, _handle, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Faile to create window surface");

        }
    }
}; //namespace Divide