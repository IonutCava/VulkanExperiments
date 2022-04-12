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
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        _handle = glfwCreateWindow(_width, _height, _name.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(_handle, this);
        glfwSetFramebufferSizeCallback(_handle, framebufferResizeCallback);
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, _handle, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Faile to create window surface");
        }
    }

    void Window::framebufferResizeCallback(GLFWwindow* window, const int width, const int height) {
        auto windowHandle = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        windowHandle->_framebufferResized = true;
        windowHandle->_width = width;
        windowHandle->_height = height;
    }
}; //namespace Divide