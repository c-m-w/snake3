/// snake3.hpp

#include <iostream>

// GLFW

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

// vulkan

#include <vulkan/vulkan.hpp>

// snake3

#include "utils/singleton.hpp"

#include "drawing/window.hpp"
#include "drawing/render.hpp"