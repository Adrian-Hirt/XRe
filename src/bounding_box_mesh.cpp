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
  vertex_count = vertices.size();
  index_count = bounding_box_indices.size();

  createVertexBuffer(vertices, &vertex_buffer);
  createIndexBuffer(bounding_box_indices, &index_buffer);
}

void BoundingBoxMesh::render() {
  UINT stride = sizeof(vertex_t);
  UINT offset = 0;

  device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
  device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
  device_context->PSSetShaderResources(0, 1, &nulltexture);
  device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
  device_context->DrawIndexed(24, 0, 0);
}
