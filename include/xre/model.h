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
  void render(Shader &shader);
  void renderTransparent(Shader &shader);
  void renderInSceneNode();

  DirectX::XMMATRIX getTransformationMatrix();
  DirectX::XMMATRIX getRotationMatrix();

  // These methods apply the rotation / translation / scaling
  // to the values we already have, e.g. to simply double the
  // scale you could pass in a value of 2 to the `scale` method
  void rotate(float roll, float pitch, float yaw);
  void rotate(DirectX::XMVECTOR rotation);
  void translate(float x, float y, float z);
  void translate(DirectX::XMVECTOR translation);
  void scale(float x, float y, float z);
  void scale(DirectX::XMVECTOR scaling);

  // These methods set the rotation / translation / scaling to
  // the values passed in, use this if you already calculated the
  // new values to use beforehand.
  void setRotation(DirectX::XMVECTOR rotation);
  void setScale(float x, float y, float z);
  void setScale(DirectX::XMVECTOR scaling);
  void setPosition(float x, float y, float z);
  void setPosition(DirectX::XMVECTOR position);

  // Getters for position, scale and rotation
  DirectX::XMVECTOR getRotation();
  DirectX::XMVECTOR getScale();
  DirectX::XMVECTOR getPosition();

  // Set the color of the model
  void setColor(DirectX::XMFLOAT4 color);
  void resetColor();
  DirectX::XMFLOAT4 getColor();

  static void registerDx11Handler(Dx11Handler *handler);

  std::vector<DirectX::XMFLOAT3> getMeshBoundingBoxCorners();

private:
  // Pointer to the Dx11 handler
  inline static Dx11Handler *s_dx11_handler = NULL;

  // Track whether a model is currently grabbed (e.g. to stop animations when object is grabbed)
  bool m_grabbed = false;

  // Vector holding all the meshes of this model
  std::vector<Mesh> m_meshes;

  // Position, scale and rotation of the model
  // Scale factors, initialize to use the scale of 1 for X, Y and Z
  DirectX::XMVECTOR m_scaling = DirectX::XMVECTORF32({1.0f, 1.0f, 1.0f});
  // The position of the object in world coordinates
  DirectX::XMVECTOR m_translation = DirectX::XMVECTORF32({0.0f, 0.0f, 0.0f});
  // The rotation of the model
  DirectX::XMVECTOR m_rotation = DirectX::XMQuaternionIdentity();

  // Color of the model, which will be applied to all meshes
  DirectX::XMFLOAT4 m_model_color;

  // Store the original color
  DirectX::XMFLOAT4 m_original_model_color;

  void loadObj(const char *model_path);
};
