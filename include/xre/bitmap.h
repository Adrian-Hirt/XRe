#pragma once

// XRe includes
#include <xre/mesh.h>
#include <xre/shader.h>

class Bitmap : public Mesh {
public:
  Bitmap(float top_left_position_x, float top_left_position_y, float x_extend, float y_extend, const char *texture_path);

  void render() override;

private:
  Shader shader;
};