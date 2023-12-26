#pragma once

// DirectX includes
#include <d3d11.h>

// Include other headers
#include <vector>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/mesh.h>

class Model {
public:
  Model();
  Model(std::vector<Mesh> meshes);
  Model(const char *model_path);

  // Two different rendering methods, one where the user can pass in
  // a shader to use for this model
  void render();
  void render(Shader *shader);

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

  static void registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context);

private:
  // Pointers to the D3D11 device and device_context which we might
  // need if we want to create meshes in the model class (e.g. when
  // loading a model from a .obj file)
  inline static ID3D11Device *device = NULL;
  inline static ID3D11DeviceContext *device_context = NULL;

  // Vector holding all the meshes of this model
  std::vector<Mesh> meshes;

  // Position, scale and rotation of the model
  // Scale factors, initialize to use the scale of 1 for X, Y and Z
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({1.0f, 1.0f, 1.0f});
  // The position of the object in world coordinates
  DirectX::XMVECTOR translation = DirectX::XMVECTORF32({0.0f, 0.0f, 0.0f});
  // The rotation of the model
  DirectX::XMVECTOR rotation = DirectX::XMQuaternionIdentity();

  void loadObj(const char *model_path);
};
