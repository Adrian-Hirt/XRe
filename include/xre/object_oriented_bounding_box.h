#pragma once

// GLM includes
#include <glm/glm/vec3.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm/gtx/quaternion.hpp>

// Other includes
#include <vector>
#include <limits>
#include <iostream>

class OOBB {
public:
  OOBB();
  OOBB(const std::vector<glm::vec3>& points);

  bool intersects(OOBB& other);
  OOBB transformed(const glm::mat4& model) const;
  std::vector<glm::vec3> getCorners() const;
  std::vector<uint16_t> getLineIndices() const;

  glm::vec3 getCenter();
  glm::vec3 getExtents();
  glm::mat3 getAxes();

  void print();

private:
  glm::vec3 m_center;
  glm::vec3 m_extents;
  glm::mat3 m_axes;

  glm::vec3 powerIteration(const glm::mat3& A, int maxIterations);
};
