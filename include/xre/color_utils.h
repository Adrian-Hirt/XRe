#pragma once

// XRe includes
#include <xre/utils.h>

// GLM includes
#include <glm/glm/glm.hpp>

namespace ColorUtils {
  inline static glm::vec3 lighten(glm::vec3 color, float factor) {
    return glm::mix(color, glm::vec3(1.0f), factor);
  }

  inline static glm::vec3 darken(glm::vec3 color, float factor) {
    return glm::mix(color, glm::vec3(0.0f), factor);
  }
}
