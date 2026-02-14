#pragma once

// Include other headers
#include <vector>
#include <glm/glm/vec3.hpp>
#include <memory>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/mesh.h>
#include <xre/geometry.h>
#include <xre/color_utils.h>
#include <xre/material.h>

class Model {
public:
  Model(std::vector<std::shared_ptr<Mesh>> meshes, glm::vec3 color, std::shared_ptr<Material> material);
  Model(const char *model_path, glm::vec3 color, std::shared_ptr<Material> material, std::shared_ptr<VulkanHandler> vulkan_handler);

  // Set the color of the model
  void setColor(glm::vec3 color);
  void resetColor();
  glm::vec3 getColor();

  // Intersection check
  bool intersects(std::shared_ptr<Model> other, glm::mat4 other_scene_node_transform, glm::mat4 scene_node_transform);
  bool intersects(const glm::vec3 &line_start, const glm::vec3 &line_direction, float *out_distance, glm::mat4 scene_node_transform);

  // Debug methods
  void toggleRenderBoundingBoxes();
  void printBouindingBoxes();

  // Toggling the state if a model is intersected and therefore
  // should have a slightly different color applied
  void setInteractedState(bool interacted);

  const std::vector<std::vector<glm::vec3>> getVectorPositionsPerMesh() const;

private:
  // The index of the current model itself to index into the buffer of the material
  uint32_t m_model_index;

  // Vector holding all the meshes of this model
  std::vector<std::shared_ptr<Mesh>> m_meshes;

  // Color of the model, which will be applied to all meshes
  glm::vec3 m_model_color;

  // Store the original color
  glm::vec3 m_original_model_color;

  void loadObj(const char *model_path, std::shared_ptr<VulkanHandler> vulkan_handler);
  void render(RenderContext &ctx, glm::mat4 scene_node_transform);

  bool m_render_bounding_boxes = false;

  bool m_interacted = false;

  std::shared_ptr<Material> m_material;

  // Scene Node can call render() directly
  friend class SceneNode;
};
