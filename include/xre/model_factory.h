#pragma once

#include <xre/mesh.h>
#include <xre/model.h>
#include <tuple>

class ModelFactory {
public:
  ModelFactory();

  static Model createCube(DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
  static Model createGroundPlane(float extent);
  static Model createGroundPlane(float extent, const char *texture_path);

private:
  static std::tuple<std::vector<vertex_t>, std::vector<unsigned int>> getGroundVerticesAndIndices(float extent);
};
