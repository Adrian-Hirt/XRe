#pragma once

// Include vector header
#include <vector>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/renderable.h>

class Mesh : public Renderable {
public:
  Mesh();
  Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices);

private:
  void render(RenderContext &ctx);

  // Only Model can call Mesh::render()
  friend class Model;
};
