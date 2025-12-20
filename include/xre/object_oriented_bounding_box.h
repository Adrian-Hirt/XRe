#pragma once

// GLM includes
#include <glm/glm/vec3.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

// Other includes
#include <vector>
#include <limits>

class OOBB {
public:
  OOBB(const std::vector<glm::vec3>& points);

  glm::vec3 getCenter();
  glm::vec3 getExtents();
  glm::mat3 getAxes();

private:
  glm::vec3 m_center;
  glm::vec3 m_extents;
  glm::mat3 m_axes;

  glm::vec3 powerIteration(const glm::mat3& A, int maxIterations);
};
