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
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

/**************************************************
 *
 *				       json
 *
 **************************************************/

#include <json/single_include/nlohmann/json.hpp>

/**************************************************
 *
 *               standard library
 *
 **************************************************/

#include <algorithm>
#include <array>
#include <chrono>
#include <fstream>
#include <map>
#include <queue>
#include <optional>
#include <thread>
#include <iostream>
#include <set>
#include <vector>

/**************************************************
 *
 *				       vulkan
 *
 **************************************************/

#include <vulkan/vulkan.hpp>

/**************************************************
 *
 *               		snake3
 *
 **************************************************/

#include "helpers/singleton.hpp"
#include "helpers/utils.hpp"
#include "helpers/dependable.hpp"
#include "helpers/loggable.hpp"
#include "helpers/settings.hpp"

#include "drawing/window.hpp"

#include "engine/input.hpp"

#include "drawing/render/queue_families.hpp"
#include "drawing/render/swap_info.hpp"
#include "drawing/render/vertex.hpp"
#include "drawing/render/uniform_buffer.hpp"
#include "drawing/render/camera.hpp"
#include "drawing/render/i_renderable.hpp"
#include "drawing/render/render.hpp"

#include "engine/grid.hpp"
