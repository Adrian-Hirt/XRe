#pragma once

#include <xre/mesh.h>
#include <xre/model.h>
#include <tuple>

class ModelFactory {
public:
  ModelFactory();
  ModelFactory(ID3D11Device *device, ID3D11DeviceContext *device_context);

  Model createCube(DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
  Model createGroundPlane(float extent);
  Model createGroundPlane(float extent, const char *texture_path);

private:
  // D3D11 device and device context
  ID3D11Device *device;
  ID3D11DeviceContext *device_context;

  std::tuple<std::vector<vertex>, std::vector<unsigned int>> getGroundVerticesAndIndices(float extent);
};
