#include <xre/bounding_box_mesh.h>

BoundingBoxMesh::BoundingBoxMesh() {}

BoundingBoxMesh::BoundingBoxMesh(std::vector<vertex_t> vertices) {
  std::vector<unsigned int> bounding_box_indices = {
      0, 1, 2, 3, 0, 3, 1, 2,
      4, 5, 6, 7, 4, 7, 5, 6,
      0, 4, 1, 5, 2, 6, 3, 7
    };

  // Store number of vertices and indices
  m_vertex_count = vertices.size();
  m_index_count = bounding_box_indices.size();

  createVertexBuffer(vertices, &m_vertex_buffer);
  createIndexBuffer(bounding_box_indices, &m_index_buffer);

  // Set shader to simply use the color shader
  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/fixed_position.hlsl");
}

void BoundingBoxMesh::render() {
  m_shader.activate();
  m_shader.setModelColor({1.0f, 1.0f, 1.0f, 1.0f});

  UINT stride = sizeof(vertex_t);
  UINT offset = 0;

  s_device_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
  s_device_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
  s_device_context->PSSetShaderResources(0, 1, &s_nulltexture);
  s_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
  s_device_context->DrawIndexed(24, 0, 0);
}

void BoundingBoxMesh::updateVerticesFromBoundingBox(DirectX::BoundingOrientedBox bounding_box) {
  // Create the bounding box mesh, such that we can render it
  DirectX::XMFLOAT3 corners[bounding_box.CORNER_COUNT];
  bounding_box.GetCorners(corners);
  std::vector<vertex_t> bounding_box_vertices;

  // Create vertices from the corners of the bounding box
  for (int i = 0; i < bounding_box.CORNER_COUNT; i++) {
    bounding_box_vertices.push_back({ corners[i], { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } });
  }

  // Update the vertex buffer
  D3D11_MAPPED_SUBRESOURCE resource;
  s_device_context->Map(m_vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
  memcpy(resource.pData, bounding_box_vertices.data(), sizeof(vertex_t) * bounding_box_vertices.size());
  s_device_context->Unmap(m_vertex_buffer, 0);
}
