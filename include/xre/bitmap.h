#pragma once

// XRe includes
#include <xre/mesh.h>
#include <xre/shader.h>

// Other includes
#include <vector>

class Bitmap : public Renderable {
public:
  Bitmap();
  Bitmap(float top_left_position_x, float top_left_position_y, float x_extend, float y_extend, const char *texture_path);
  Bitmap(std::vector<vertex_t> vertices, std::vector<unsigned int> indices, const char *texture_path);

  void render() override;

private:
  // A bitmap does not have a bounding box
  inline bool hasBoundingBox() { return false; }

  Shader m_shader;
  DirectX::XMFLOAT4 m_bitmap_color;
};