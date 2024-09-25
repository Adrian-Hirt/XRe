#pragma once

// DirectX includes
#include <DirectXTex/WICTextureLoader11.h>
#include <DirectXMath/DirectXCollision.h>

// Include vector header
#include <vector>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/shader.h>
#include <xre/renderable.h>

// Mesh class, usually not used directly, but either as a part of a `Model` class instance,
// or as a a subclass, e.g. Bitmap or Line.
class Mesh : public Renderable {
public:
  Mesh();
  Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices);
  Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices, const char *texture_path);
};
