#include "Game/Salty/SaltyCamera.h"

#include <glm.hpp>

glm::vec2 Camera::position = glm::vec2(0,0);
float Camera::rotation = 0;

glm::ivec2 Camera::aspectRatio = glm::vec2(0,0);
float Camera::scale = 1;