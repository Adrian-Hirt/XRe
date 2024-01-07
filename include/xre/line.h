#pragma once

#include <xre/mesh.h>

class Line : public Mesh {
public:
  Line(DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
  Line(DirectX::XMFLOAT3 line_start, DirectX::XMFLOAT3 line_end, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

  void render() override;
  void render(Shader *shader);
  void updateLineFromXrPose(XrPosef pose);

private:
  std::vector<vertex_t> verticesFromPoints(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color);
};