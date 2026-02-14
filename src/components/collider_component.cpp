#include <xre/components/collider_component.h>
#include <xre/scene_node.h>

ColliderComponent::ColliderComponent(std::vector<OOBB> bounding_boxes) {
  m_bounding_boxes = bounding_boxes;
}

std::unique_ptr<ColliderComponent> ColliderComponent::fromPointGroups(const std::vector<std::vector<glm::vec3>>& point_groups) {
  std::vector<OOBB> oobbs;
  oobbs.reserve(point_groups.size());

  for (const auto& points : point_groups) {
    oobbs.push_back(OOBB(points));
  }

  return std::make_unique<ColliderComponent>(std::move(oobbs));
}

bool ColliderComponent::intersects(ColliderComponent* other) {
  for (const auto& this_bounding_box : m_bounding_boxes) {
    auto this_oobb = this_bounding_box.transformed(m_node->getWorldTransform());
    for (const auto& other_bounding_box : other->m_bounding_boxes) {
      auto other_oobb = other_bounding_box.transformed(other->getSceneNode()->getWorldTransform());
      if (this_oobb.intersects(other_oobb)) {
        return true;
      }
    }
  }

  return false;
}

bool ColliderComponent::intersects(const glm::vec3 &line_start, const glm::vec3 &line_direction, float *out_distance) {
  for (const auto& this_bounding_box : m_bounding_boxes) {
    auto this_oobb = this_bounding_box.transformed(m_node->getWorldTransform());
    if (this_oobb.intersects(line_start, line_direction, out_distance)) {
      return true;
    }
  }
  return false;
}
