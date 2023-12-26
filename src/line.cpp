#include <xre/line.h>

Line::Line(ID3D11Device *device, ID3D11DeviceContext *device_context, DirectX::XMFLOAT4 color) {
  this->device = device;
  this->device_context = device_context;

  std::vector<vertex_t> vertices = verticesFromPoints({0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, color);
  std::vector<unsigned int> indices = {0, 1};

  initialize(device, device_context, vertices, indices);
};

Line::Line(ID3D11Device *device, ID3D11DeviceContext *device_context, DirectX::XMFLOAT3 line_start, DirectX::XMFLOAT3 line_end, DirectX::XMFLOAT4 color) {
  this->device = device;
  this->device_context = device_context;

  std::vector<vertex_t> vertices = verticesFromPoints(line_start, line_end, color);
  std::vector<unsigned int> indices = {0, 1};

  initialize(device, device_context, vertices, indices);
};

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
};

std::vector<vertex_t> Line::verticesFromPoints(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT4 color) {
  std::vector<vertex_t> vertices = {
    {  start, color,  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    {  end,   color,  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }
  };

  return vertices;
};
