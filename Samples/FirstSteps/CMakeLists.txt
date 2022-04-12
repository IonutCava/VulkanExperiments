# CMakeList.txt : CMake project for FirstSteps, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("FirstSteps")
# Add source to this project's executable.
add_executable (FirstSteps "FirstSteps.cpp" "FirstSteps.h")

# C/C++ languages required.
enable_language(C)
enable_language(CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.204.1")
find_package(Vulkan REQUIRED)
target_include_directories(FirstSteps PUBLIC "C:/VulkanSDK/1.3.204.1/Include")
target_link_libraries(FirstSteps ${Vulkan_LIBRARIES})

# config glfw
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(Libraries/GLFW)
target_link_libraries(FirstSteps glfw)

include_directories(Libraries/glm)

