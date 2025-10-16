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

  void render(RenderContext& ctx);

  // // Set the color of the model
  // void setColor(DirectX::XMFLOAT4 color);
  // void resetColor();
  // DirectX::XMFLOAT4 getColor();

  // std::vector<DirectX::XMFLOAT3> getMeshBoundingBoxCorners();

  // Store the translation
  glm::vec3 m_translation = glm::zero<glm::vec3>();

  // Store the rotation
  glm::quat m_rotation = glm::identity<glm::quat>();

  // Store the scaling
  glm::vec3 m_scaling = glm::one<glm::vec3>();

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

  void loadObj(const char *model_path);
};
