#include <xre/line.h>

Line::Line(DirectX::XMFLOAT4 color) {
  std::vector<vertex_t> vertices = verticesFromPoints({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, color);
  std::vector<unsigned int> indices = {0, 1};

  initialize(vertices, indices);
}

Line::Line(DirectX::XMFLOAT3 line_start, DirectX::XMFLOAT3 line_end, DirectX::XMFLOAT4 color) {
  std::vector<vertex_t> vertices = verticesFromPoints(line_start, line_end, color);
  std::vector<unsigned int> indices = {0, 1};

  initialize(vertices, indices);
}

void Line::render() {
  // Get the current active shader
  Shader* shader = Shader::getCurrentActiveShader();

  render(shader);
}

void Line::render(Shader *shader) {
  // Activate the shader
  shader->activate();

  shader->setModelMatrix(DirectX::XMMatrixIdentity());
  shader->updatePerModelConstantBuffer();

  // Set vertex and index buffers on the GPU to be the ones of this line
  UINT stride = sizeof(vertex_t);
  UINT offset = 0;
  device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
  device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

  // We'll be rendering a line list, so we need to tell the GPU
  // to render the vertices as such.
  device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

  // And finally, we can call the actual draw method, which will draw all the
  // indices of this line.
  device_context->DrawIndexed(index_count, 0, 0);
}

void Line::updateLineFromXrPose(XrPosef pose) {
  // The start of the line is simply the position of the pose
  DirectX::XMFLOAT3 line_start = *((DirectX::XMFLOAT3 *)&pose.position);

  // For the end of the line, we assume that we're aiming in negative z direction initially. As such,
  // we first take a vector in negative z direction, and then rotate it with the quaternion
  // given in the XrPosef struct
  DirectX::XMVECTOR orientation = DirectX::XMLoadFloat4((DirectX::XMFLOAT4 *)&pose.orientation);
  DirectX::XMVECTOR direction = DirectX::XMVector3Rotate(DirectX::XMVECTORF32({0.0f, 0.0f, -1.5f}), orientation);
  DirectX::XMVECTOR line_end_vec = DirectX::XMVectorAdd(direction, DirectX::XMLoadFloat3(&line_start));

  // Store the vector in an xmfloat3
  DirectX::XMFLOAT3 line_end;
  DirectX::XMStoreFloat3(&line_end, line_end_vec);

  // Create the new vertices
  std::vector<vertex_t> vertices = verticesFromPoints(line_start, line_end,  {1.0f, 1.0f, 1.0f, 1.0f});

  // Update the vertex buffer
  D3D11_MAPPED_SUBRESOURCE resource;
  device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
  memcpy(resource.pData, vertices.data(), sizeof(vertex_t) * vertices.size());
  device_context->Unmap(vertex_buffer, 0);
}

std::vector<vertex_t> Line::verticesFromPoints(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color) {
  std::vector<vertex_t> vertices = {
    {  start, color,  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    {  end,   color,  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }
  };

  return vertices;
}
