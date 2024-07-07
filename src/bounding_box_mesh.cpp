#include <xre/bounding_box_mesh.h>

BoundingBoxMesh::BoundingBoxMesh() {

}

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
}

void BoundingBoxMesh::render() {
  UINT stride = sizeof(vertex_t);
  UINT offset = 0;

  s_device_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
  s_device_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);
  s_device_context->PSSetShaderResources(0, 1, &s_nulltexture);
  s_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
  s_device_context->DrawIndexed(24, 0, 0);
}
