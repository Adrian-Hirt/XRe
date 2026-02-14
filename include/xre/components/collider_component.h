#pragma once

// XRe includes
#include <xre/object_oriented_bounding_box.h>
#include <xre/components/component.h>

// GLM includes
#include <glm/glm/vec3.hpp>

// Other includes
#include <memory>

class ColliderComponent : public Component {
public:
  ColliderComponent() = delete;
  ColliderComponent(std::vector<OOBB> bounding_boxes);

  static std::unique_ptr<ColliderComponent> fromPointGroups(const std::vector<std::vector<glm::vec3>>& point_groups);

  bool intersects(ColliderComponent* other);
  bool intersects(const glm::vec3 &line_start, const glm::vec3 &line_direction, float *out_distance);

private:
  std::vector<OOBB> m_bounding_boxes;
};
