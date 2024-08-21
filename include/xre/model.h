#pragma once

// DirectX includes
#include <d3d11.h>
#include <DirectXMath/DirectXMath.h>

// Include other headers
#include <vector>
#include <unordered_set>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/dx11_handler.h>
#include <xre/mesh.h>
#include <xre/bounding_box_mesh.h>

class Model {
public:
  Model();
  Model(std::vector<Mesh> meshes, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));
  Model(const char *model_path, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f));

  // Two different rendering methods, one where the user can pass in
  // a shader to use for this model
  void render();
  void renderTransparent();

  // Set the color of the model
  void setColor(DirectX::XMFLOAT4 color);
  void resetColor();
  DirectX::XMFLOAT4 getColor();

  static void registerDx11Handler(Dx11Handler *handler);

  std::vector<DirectX::XMFLOAT3> getMeshBoundingBoxCorners();

private:
  // Pointer to the Dx11 handler
  inline static Dx11Handler *s_dx11_handler = NULL;

  // Vector holding all the meshes of this model
  std::vector<Mesh> m_meshes;

  // Color of the model, which will be applied to all meshes
  DirectX::XMFLOAT4 m_model_color;

  // Store the original color
  DirectX::XMFLOAT4 m_original_model_color;

  void loadObj(const char *model_path);
};
