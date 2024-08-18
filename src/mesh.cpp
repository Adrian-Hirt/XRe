#include <xre/mesh.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Mesh::Mesh() {}

//------------------------------------------------------------------------------------------------------
// Initialize the mesh.
// Arguments:
//  1) Vector with the vertices of the mesh
//  2) Vector with the incides of the mesh
//------------------------------------------------------------------------------------------------------
Mesh::Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices) {
  // Call general initialize method
  initialize(vertices, indices);
}

//------------------------------------------------------------------------------------------------------
// Initialize the mesh.
// Arguments:
//  1) Vector with the vertices of the mesh
//  2) Vector with the incides of the mesh
//  3) Path to the file holding the texture to be applied
//------------------------------------------------------------------------------------------------------
Mesh::Mesh(std::vector<vertex_t> vertices, std::vector<unsigned int> indices, const char *texture_path) {
  // Call general initialize method
  initialize(vertices, indices);

  // Load the texture
  std::wstring filepath = Utils::stringToWString(texture_path);
  ID3D11Resource *texture; // Throwaway variable
  // Use the `Ex` variant, which allows us to set the loader to assume that
  // the passed in texture is already sRGB.
  HRESULT result = DirectX::CreateWICTextureFromFileEx(
    s_device,
    s_device_context,
    filepath.c_str(),
    0,
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_SHADER_RESOURCE,
    0,
    0,
    DirectX::WIC_LOADER_SRGB_DEFAULT,
    &texture,
    &m_texture_view
  );

  std::string error_string = "Failed to load the texture ";
  error_string += texture_path;
  Utils::checkHresult(result, error_string.c_str());
}

void Mesh::render() {
  // Set vertex and index buffers on the GPU to be the ones of this mesh
  UINT stride = sizeof(vertex_t);
  UINT offset = 0;
  s_device_context->IASetVertexBuffers(0, 1, &m_vertex_buffer, &stride, &offset);
  s_device_context->IASetIndexBuffer(m_index_buffer, DXGI_FORMAT_R32_UINT, 0);

  // Set the texture if it exists, otherwise remove any previously added textures
  if (m_texture_view) {
    s_device_context->PSSetShaderResources(0, 1, &m_texture_view);
  }
  else {
    s_device_context->PSSetShaderResources(0, 1, &s_nulltexture);
  }

  // We'll be rendering a triangle list, so we need to tell the GPU
  // to render the vertices as such.
  s_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // And finally, we can call the actual draw method, which will draw all the
  // indices of this mesh.
  s_device_context->DrawIndexed(m_index_count, 0, 0);
}
