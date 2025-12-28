#pragma once

// XRe includes
#include <xre/model.h>
#include <xre/scene_node.h>
#include <xre/model_factory.h>
#include <xre/structs.h>
#include <xre/vulkan_handler.h>

// Other includes
#include <memory>

class Line {
public:
  Line(float thickness, float length, glm::vec3 color, std::shared_ptr<Material> material, std::shared_ptr<VulkanHandler> vulkan_handler);

  void render(RenderContext &ctx);
  void updateAimLineFromControllerPose(glm::vec3 controller_position, glm::quat controller_orientation, float length);
  glm::vec3 getLineStart();
  glm::vec3 getLineDirection();

private:
  // Model for the line
  std::shared_ptr<Model> m_model;

  // Scene node for the line
  SceneNode m_scene_node;

  glm::vec3 m_line_color;
  glm::vec3 m_line_start = glm::zero<glm::vec3>();
  glm::vec3 m_line_direction = {0.0f, 0.0f, -1.0f};
};