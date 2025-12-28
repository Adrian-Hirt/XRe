#pragma once

// XRe includes
#include <xre/model.h>
#include <xre/scene_node.h>
#include <xre/model_factory.h>
#include <xre/structs.h>

class Line {
public:
  Line();
  Line(float thickness, float length, Material* material);
  Line(float thickness, float length, glm::vec3 color, Material* material);

  void render(RenderContext &ctx);
  void updateAimLineFromControllerPose(glm::vec3 controller_position, glm::quat controller_orientation, float length);
  glm::vec3 getLineStart();
  glm::vec3 getLineDirection();

private:
  // Model for the line
  Model *m_model;

  // Scene node for the line
  SceneNode m_scene_node;

  glm::vec3 m_line_color;
  glm::vec3 m_line_start = glm::zero<glm::vec3>();
  glm::vec3 m_line_direction = {0.0f, 0.0f, -1.0f};
};