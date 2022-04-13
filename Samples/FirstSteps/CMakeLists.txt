# CMakeList.txt : CMake project for FirstSteps, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("FirstSteps")
# Add source to this project's executable.
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/Src/*.cpp)

add_executable (FirstSteps ${SOURCES})
target_include_directories(FirstSteps PUBLIC ${PROJECT_SOURCE_DIR}/Src)

# C/C++ languages required.
enable_language(C)
enable_language(CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

target_compile_features(FirstSteps PRIVATE cxx_std_17)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")
IF(MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF(MSVC)

set(ENV{VULKAN_SDK} "C:/VulkanSDK/1.3.204.1")

#Auto shader compile GLSL -> SPIR-V
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)
set(SHADER_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Shaders)
set(SHADER_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Shaders)
file(GLOB SHADERS
  ${SHADER_SOURCE_DIR}/*.vert
  ${SHADER_SOURCE_DIR}/*.frag
  ${SHADER_SOURCE_DIR}/*.comp
  ${SHADER_SOURCE_DIR}/*.geom
  ${SHADER_SOURCE_DIR}/*.tesc
  ${SHADER_SOURCE_DIR}/*.tese
  ${SHADER_SOURCE_DIR}/*.mesh
  ${SHADER_SOURCE_DIR}/*.task
  ${SHADER_SOURCE_DIR}/*.rgen
  ${SHADER_SOURCE_DIR}/*.rchit
  ${SHADER_SOURCE_DIR}/*.rmiss)
add_custom_command(
  COMMAND
    ${CMAKE_COMMAND} -E make_directory ${SHADER_BINARY_DIR}
  OUTPUT ${SHADER_BINARY_DIR}
  COMMENT "Creating ${SHADER_BINARY_DIR}"
)

set(ASSETS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Assets)
set(ASSETS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Assets)

add_custom_command(TARGET FirstSteps PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${ASSETS_SOURCE_DIR} ${ASSETS_BINARY_DIR})

foreach(source IN LISTS SHADERS)
  get_filename_component(FILENAME ${source} NAME)
  add_custom_command(
    COMMAND
      ${glslc_executable}
      #      -MD -MF ${SHADER_BINARY_DIR}/${FILENAME}.d
      -o ${SHADER_BINARY_DIR}/${FILENAME}.spv
      ${source}
    OUTPUT ${SHADER_BINARY_DIR}/${FILENAME}.spv
    DEPENDS ${source} ${SHADER_BINARY_DIR}
    COMMENT "Compiling ${FILENAME}"
  )
  list(APPEND SPV_SHADERS ${SHADER_BINARY_DIR}/${FILENAME}.spv)
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPV_SHADERS})
target_compile_features(FirstSteps PRIVATE cxx_std_17)

#Add Vulkan SDK
find_package(Vulkan REQUIRED)
IF (Vulkan_LIBRARY)
    set(Vulkan_FOUND ON)
    MESSAGE("Using bundled Vulkan library version")
ENDIF()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_USE_PLATFORM_WIN32_KHR")
IF (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
ELSE()
    message(STATUS ${Vulkan_LIBRARY})
ENDIF()
target_include_directories(FirstSteps PUBLIC "C:/VulkanSDK/1.3.204.1/Include")
target_link_libraries(FirstSteps ${Vulkan_LIBRARIES})

# Add and config GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(Libraries/GLFW)
target_link_libraries(FirstSteps glfw)

# Add and config GLM
include_directories(Libraries/glm)

# Add and config TinyObjLoader
include_directories(Libraries/tinyobjloader)