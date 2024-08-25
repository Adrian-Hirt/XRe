#pragma once

#include <xre/mesh.h>

class BoundingBoxMesh : public Mesh {
public:
  BoundingBoxMesh();
  BoundingBoxMesh(std::vector<vertex_t> vertices);

  void render() override;
  void updateVerticesFromBoundingBox(DirectX::BoundingOrientedBox bounding_box);

private:
  inline bool usesStaticBuffers() { return false; };
};
