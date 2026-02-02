#ifndef CONFIG_H
#define CONFIG_H

#include <glm/glm.hpp>

// window configuration
#define INITIAL_WINDOW_WIDTH 700
#define INITIAL_WINDOW_HEIGHT 700

// rendering configuration
const float NEAR_PLANE = 1.0f;
const float FAR_PLANE = 20000.0f;

// camera configuration
const glm::vec3 CAMERA_START_POSITION = glm::vec3(0.0f, 0.0f, 3.0f);
const float CAMERA_NORMAL_SPEED = 2.5f;
const float CAMERA_FAST_SPEED = 50.0f;
const float CAMERA_SLOW_SPEED = 0.5f;

// scale configuration
const float PLANET_SIZE_SCALE = 1.0f;
const float DISTANCE_SCALE = 1.0f;
const float SPEED_SCALE = 1.0f;

// sun properties
const float SUN_SIZE = 2.0f;
const char *SUN_TEXTURE = "assets/textures/2k_sun.jpg";

// background properties
const float BACKGROUND_SIZE = 8000.0f;
const char *BACKGROUND_TEXTURE = "assets/textures/8k_stars_milky_way.jpg";

// moon properties
const float MOON_SIZE = 0.273f * PLANET_SIZE_SCALE;
const float MOON_ORBIT_RADIUS = 0.087f;
const float MOON_ORBIT_SPEED = 13.4f;
const char *MOON_TEXTURE = "assets/textures/2k_moon.jpg";

// orbit rendering
const glm::vec3 ORBIT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);

// sun light intensities
const glm::vec3 LIGHT_AMBIENT = glm::vec3(0.15f, 0.15f, 0.15f);
const glm::vec3 LIGHT_DIFFUSE = glm::vec3(1.0f, 1.0f, 1.0f);
const glm::vec3 LIGHT_SPECULAR = glm::vec3(1.0f, 1.0f, 1.0f);

// rocky planet material
const glm::vec3 ROCKY_KA = glm::vec3(0.25f, 0.25f, 0.25f);
const glm::vec3 ROCKY_KD = glm::vec3(0.8f, 0.8f, 0.8f);
const glm::vec3 ROCKY_KS = glm::vec3(0.3f, 0.3f, 0.3f);
const float ROCKY_SHININESS = 32.0f;

// gas giant material
const glm::vec3 GAS_KA = glm::vec3(0.3f, 0.3f, 0.3f);
const glm::vec3 GAS_KD = glm::vec3(0.9f, 0.9f, 0.9f);
const glm::vec3 GAS_KS = glm::vec3(0.5f, 0.5f, 0.5f);
const float GAS_SHININESS = 64.0f;

#endif
