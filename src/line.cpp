#include <xre/line.h>

Line::Line() {}
Line::Line(float thickness, float length) : Line::Line(thickness, length, {0.0f, 0.0f, 0.0f}) {}
Line::Line(float thickness, float length, glm::vec3 color) {
  m_line_color = color;
  m_model = ModelFactory::createCubePtr(m_line_color);
  m_scene_node = SceneNode(m_model);
  m_scene_node.setScale({thickness, thickness, length});
  m_scene_node.setPosition({0.0f, 1.0f, 0.0f});
}

void Line::render(RenderContext &ctx) {
  m_scene_node.updateTransformation();
  m_scene_node.render(ctx);
}

void Line::updateAimLineFromControllerPose(glm::vec3 controller_position,
                                           glm::quat controller_orientation,
                                           glm::vec3 current_origin,
                                           float length) {
  // Compute direction from OpenXR quaternion
  glm::vec3 forward = { 0.0f, 0.0f, -1.0f };
  glm::vec3 normalized_direction = glm::normalize(controller_orientation * forward);

  // Compute rotation to align cube's +Z axis with dir
  glm::vec3 cube_forward(0.0f, 0.0f, 1.0f);
  glm::quat rotation = glm::rotation(cube_forward, normalized_direction);
  m_scene_node.setRotation(rotation);

  // Compute offset for cube center: move forward by the length
  glm::vec3 cube_center = controller_position + normalized_direction * length;
  m_scene_node.setPosition(cube_center);

  // Update fields
  m_line_start = controller_position;
  m_line_direction = normalized_direction;

  // Update Z scale for the length
  glm::vec3 scale = m_scene_node.getScale();
  scale.z = length;
  m_scene_node.setScale(scale);
}

glm::vec3 Line::getLineStart() {
  return m_line_start;
}

glm::vec3 Line::getLineDirection() {
  return m_line_direction;
}
