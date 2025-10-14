#pragma once

// Include other headers
#include <vector>
#include <glm/glm/vec3.hpp>
// #include <unordered_set>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/mesh.h>
// #include <xre/bounding_box_mesh.h>

class Model {
public:
  Model();
  Model(std::vector<Mesh*> meshes);
  Model(std::vector<Mesh*> meshes, glm::vec3 color);

  Model(const char *model_path);
  Model(const char *model_path, glm::vec3 color);

  void render(RenderContext& ctx);

  // // Set the color of the model
  // void setColor(DirectX::XMFLOAT4 color);
  // void resetColor();
  // DirectX::XMFLOAT4 getColor();

  // std::vector<DirectX::XMFLOAT3> getMeshBoundingBoxCorners();

private:
  // Vector holding all the meshes of this model
  std::vector<Mesh*> m_meshes;

  // Color of the model, which will be applied to all meshes
  glm::vec3 m_model_color;

  // Store the original color
  glm::vec3 m_original_model_color;

  void loadObj(const char *model_path);
};
