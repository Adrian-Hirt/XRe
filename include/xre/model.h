#pragma once

// Include other headers
#include <vector>
#include <glm/glm/vec3.hpp>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/mesh.h>
#include <xre/geometry.h>
#include <xre/color_utils.h>
#include <xre/material.h>

class Model {
public:
  Model();
  Model(std::vector<Mesh> meshes, Material material);
  Model(std::vector<Mesh> meshes, glm::vec3 color, Material material);

  Model(const char *model_path, Material material);
  Model(const char *model_path, glm::vec3 color, Material material);

  // Set the world transform
  void setWorldMatrix(glm::mat4 world_matrix);

  // Set the color of the model
  void setColor(glm::vec3 color);
  void resetColor();
  glm::vec3 getColor();

  // Intersection check
  bool intersects(Model other);
  bool intersects(const glm::vec3 &line_start, const glm::vec3 &line_direction, float *out_distance);

  // Debug methods
  void toggleRenderBoundingBoxes();
  void printBouindingBoxes();

  // Toggling the state if a model is intersected and therefore
  // should have a slightly different color applied
  void setInteractedState(bool interacted);

private:
  // Keep track of the global index of the model
  inline static uint32_t s_model_index = 0;

  // The index of the current model itself
  uint32_t m_model_index;

  // Vector holding all the meshes of this model
  std::vector<Mesh> m_meshes;

  // Color of the model, which will be applied to all meshes
  glm::vec3 m_model_color;

  // Store the original color
  glm::vec3 m_original_model_color;

  // Store the world transform
  glm::mat4 m_world_matrix;

  void loadObj(const char *model_path);
  void render(RenderContext &ctx);

  bool m_render_bounding_boxes = false;

  bool m_interacted = false;
  
  Material m_material;

  // Scene Node can call render() directly
  friend class SceneNode;
};
