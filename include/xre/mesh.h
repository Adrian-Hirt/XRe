#pragma once

// DirectX includes
// #include <DirectXTex/WICTextureLoader11.h>
// #include <DirectXMath/DirectXCollision.h>

// Include vector header
#include <vector>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
// #include <xre/shader.h>
#include <xre/renderable.h>

class Mesh : public Renderable {
public:
  Mesh();
  Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices);
  // Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices, const char *texture_path);

private:
  void render(RenderContext& ctx);

  // Only Model can call Mesh::render()
  friend class Model;
};
