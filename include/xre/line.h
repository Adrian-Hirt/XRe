#pragma once

// XRe includes
#include <xre/renderable.h>

class Line : public Renderable {
public:
  Line(DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
  Line(DirectX::XMFLOAT3 line_start, DirectX::XMFLOAT3 line_end, DirectX::XMFLOAT4 color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

  void render() override;
  void render(Shader &shader);
  void updateAimLineFromControllerPose(DirectX::XMVECTOR controller_position, DirectX::XMVECTOR controller_orientation, DirectX::XMVECTOR current_origin, float length);

  DirectX::XMVECTOR getLineStart();
  DirectX::XMVECTOR getLineDirection();

private:
  // A line does not have a bounding box
  inline bool hasBoundingBox() { return false; }

  inline bool usesStaticBuffers() { return false; };

  std::vector<vertex_t> verticesFromPoints(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);

  DirectX::XMFLOAT4 m_line_color;
  DirectX::XMVECTOR m_line_start = { 0.0f, 0.0f, 0.0f };
  DirectX::XMVECTOR m_line_direction = { 0.0f, 0.0f, -1.0f };
};