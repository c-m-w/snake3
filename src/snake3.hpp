/// snake3.hpp

/**************************************************
 *
 *                     GLFW
 *
 **************************************************/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

/**************************************************
 *
 *				       vulkan
 *
 **************************************************/

#include <vulkan/vulkan.hpp>

/**************************************************
 *
 *               standard library
 *
 **************************************************/

#include <algorithm>
#include <array>
#include <chrono>
#include <queue>
#include <thread>
#include <iostream>
#include <vector>

/**************************************************
 *
 *               		snake3
 *
 **************************************************/

#include "helpers/utils.hpp"
#include "helpers/singleton.hpp"
#include "helpers/loggable.hpp"

#include "drawing/window.hpp"
#include "drawing/render.hpp"
