#pragma once

// Include other headers
#include <vector>
#include <glm/glm/vec3.hpp>
// #include <unordered_set>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/mesh.h>
#include <xre/geometry.h>
// #include <xre/bounding_box_mesh.h>

class Model {
public:
  Model();
  Model(std::vector<Mesh> meshes);
  Model(std::vector<Mesh> meshes, glm::vec3 color);

  Model(const char *model_path);
  Model(const char *model_path, glm::vec3 color);

  // Set the world transform
  void setWorldMatrix(glm::mat4 world_matrix);

  // Set the color of the model
  void setColor(glm::vec3 color);
  void resetColor();
  glm::vec3 getColor();

  // std::vector<DirectX::XMFLOAT3> getMeshBoundingBoxCorners();

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
  void render(RenderContext& ctx);

  // Scene Node can call render() directly
  friend class SceneNode;
};
