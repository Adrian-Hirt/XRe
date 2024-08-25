#include <xre/line.h>

Line::Line(DirectX::XMFLOAT4 color) {
  std::vector<vertex_t> vertices = verticesFromPoints({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f});
  std::vector<unsigned int> indices = {0, 1};

  // Set the color of the line to be rendered
  m_line_color = color;

  // Set buffers to be dynamic, such that their contents can be updated
  m_static_buffers = false;

  // Set shader to simply use the color shader
  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/fixed_position.hlsl");

  initialize(vertices, indices);
}

Line::Line(DirectX::XMFLOAT3 line_start, DirectX::XMFLOAT3 line_end, DirectX::XMFLOAT4 color) {
  std::vector<vertex_t> vertices = verticesFromPoints(line_start, line_end);
  std::vector<unsigned int> indices = {0, 1};

  // Set the color of the line to be rendered
  m_line_color = color;

  // Set buffers to be dynamic, such that their contents can be updated
  m_static_buffers = false;

  // Set shader to simply use the color shader
  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/fixed_position.hlsl");

  initialize(vertices, indices);
}

void Line::render() {
  // Activate the shader
  m_shader.activate();

  m_shader.setModelColor(m_line_color);
  m_shader.updatePerModelConstantBuffer();

  // Set vertex and index buffers on the GPU to be the ones of this line
  UINT stride = sizeof(vertex_t);
  UINT offset = 0;
  s_device_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
  s_device_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

  // We'll be rendering a line list, so we need to tell the GPU
  // to render the vertices as such.
  s_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

  // And finally, we can call the actual draw method, which will draw all the
  // indices of this line.
  s_device_context->DrawIndexed(m_index_count, 0, 0);
}

void Line::updateAimLineFromControllerPose(DirectX::XMVECTOR controller_position, DirectX::XMVECTOR controller_orientation, DirectX::XMVECTOR current_origin, float length) {
  // The start of the line is simply the position of the controller
  m_line_start = controller_position;

  // For the end of the line, we assume that we're aiming in negative z direction initially. As such,
  // we first take an unit-vector in negative z direction, and then rotate it with the quaternion
  // given in the XrPosef struct
  m_line_direction = DirectX::XMVector3Rotate(DirectX::XMVECTORF32({0.0f, 0.0f, -1.0f}), controller_orientation);

  // Stretch the direction vector to have the given length. As the vector has unit length, we can simply
  // multiply each component by the passed in length.
  DirectX::XMVECTOR stretched_direction = m_line_direction * length;
  DirectX::XMVECTOR line_end_vec = DirectX::XMVectorAdd(stretched_direction, m_line_start);

  // Store the vectors in xmfloat3 to build vertices for the lines
  DirectX::XMFLOAT3 line_start;
  DirectX::XMStoreFloat3(&line_start, m_line_start);
  DirectX::XMFLOAT3 line_end;
  DirectX::XMStoreFloat3(&line_end, line_end_vec);

  // Create the new vertices
  std::vector<vertex_t> vertices = verticesFromPoints(line_start, line_end);

  // Update the vertex buffer
  D3D11_MAPPED_SUBRESOURCE resource;
  s_device_context->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
  memcpy(resource.pData, vertices.data(), sizeof(vertex_t) * vertices.size());
  s_device_context->Unmap(m_vertex_buffer, 0);
}

std::vector<vertex_t> Line::verticesFromPoints(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end) {
  std::vector<vertex_t> vertices = {
    {  start, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    {  end,   DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }
  };

  return vertices;
}

DirectX::XMVECTOR Line::getLineStart() {
  return m_line_start;
}

DirectX::XMVECTOR Line::getLineDirection() {
  return m_line_direction;
}
