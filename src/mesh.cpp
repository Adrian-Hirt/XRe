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

// Function to initialize the "common" data of a mesh, to avoid code-duplication
void Mesh::initialize(std::vector<vertex_t> vertices, std::vector<unsigned int> indices) {
  // Store number of vertices and indices
  m_vertex_count = vertices.size();
  m_index_count = indices.size();

  createVertexBuffer(vertices, &m_vertex_buffer);
  createIndexBuffer(indices, &m_index_buffer);

  if (hasBoundingBox()) {
    // Store vertex positions temporary
    DirectX::XMFLOAT3 vertex_positions[m_vertex_count];

    for(int i = 0; i < m_vertex_count; i++) {
      vertex_positions[i] = vertices[i].coordinates;
    }

    // Create the bounding box for this mesh
    DirectX::BoundingOrientedBox::CreateFromPoints(m_bounding_box, m_vertex_count, vertex_positions, sizeof(DirectX::XMFLOAT3));

    // TODO: check what this is for
    // DirectX::XMFLOAT3 corners[m_bounding_box.CORNER_COUNT];
    // m_bounding_box.GetCorners(corners);
    // std::vector<vertex_t> bounding_box_vertices;

    // // Create vertices from the corners of the bounding box
    // for (int i = 0; i < m_bounding_box.CORNER_COUNT; i++) {
    //   bounding_box_vertices.push_back({ corners[i], { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } });
    // }
  }
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

void Mesh::registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  Mesh::s_device = device;
  Mesh::s_device_context = device_context;
}

void Mesh::createVertexBuffer(std::vector<vertex_t> data, ID3D11Buffer **target_buffer) {
  D3D11_BUFFER_DESC buffer_desc;
  ZeroMemory(&buffer_desc, sizeof(buffer_desc));

  buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and read access by GPU
  buffer_desc.ByteWidth = sizeof(vertex_t) * data.size(); // Size of the buffer we want to allocate
  buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
  buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

  // Create the buffer and copy the vertices into it
  D3D11_SUBRESOURCE_DATA vertex_buffer_data = { data.data() };
  HRESULT result = s_device->CreateBuffer(&buffer_desc, &vertex_buffer_data, target_buffer);
  Utils::checkHresult(result, "Could not create the vertex buffer!");
}

void Mesh::createIndexBuffer(std::vector<unsigned int> data, ID3D11Buffer **target_buffer) {
  D3D11_BUFFER_DESC buffer_desc;
  ZeroMemory(&buffer_desc, sizeof(buffer_desc));
  buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                    // write access access by CPU and read access by GPU
  buffer_desc.ByteWidth = sizeof(unsigned int) * data.size(); // Size of the buffer we want to allocate
  buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;            // use as an index buffer
  buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;        // allow CPU to write in buffer
  buffer_desc.MiscFlags = 0;

  // Create the buffer and copy the indices into it
  D3D11_SUBRESOURCE_DATA index_buffer_data = { data.data() };
  HRESULT result = s_device->CreateBuffer(&buffer_desc, &index_buffer_data, target_buffer);
  Utils::checkHresult(result, "Could not create the vertex buffer!");
}

DirectX::BoundingOrientedBox Mesh::getBoundingBox() {
  return m_bounding_box;
}

