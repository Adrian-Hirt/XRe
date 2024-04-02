#pragma once

#include <xre/mesh.h>

class BoundingBoxMesh : public Mesh {
public:
  BoundingBoxMesh();
  BoundingBoxMesh(std::vector<vertex_t> vertices);

  void render() override;
};
