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
    device,
    device_context,
    filepath.c_str(),
    0,
    D3D11_USAGE_DEFAULT,
    D3D11_BIND_SHADER_RESOURCE,
    0,
    0,
    DirectX::WIC_LOADER_SRGB_DEFAULT,
    &texture,
    &p_texture_view
  );

  std::string error_string = "Failed to load the texture ";
  error_string += texture_path;
  Utils::checkHresult(result, error_string.c_str());
}

// Function to initialize the "common" data of a mesh, to avoid code-duplication
void Mesh::initialize(std::vector<vertex_t> vertices, std::vector<unsigned int> indices) {
  // Setup the mesh
  HRESULT result;

  // Store number of vertices and indices
  vertex_count = vertices.size();
  index_count = indices.size();

  // Store vertex positions temporary
  DirectX::XMFLOAT3 vertex_positions[vertex_count];

  for(int i = 0; i < vertex_count; i++) {
    vertex_positions[i] = vertices[i].coordinates;
  }

  // Setup the vertex buffer
  D3D11_BUFFER_DESC buffer_desc;
  ZeroMemory(&buffer_desc, sizeof(buffer_desc));

  buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                    // write access access by CPU and read access by GPU
  buffer_desc.ByteWidth = sizeof(vertex_t) * vertices.size(); // Size of the buffer we want to allocate
  buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;           // use as a vertex buffer
  buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;        // allow CPU to write in buffer

  // Create the buffer and copy the vertices into it
  D3D11_SUBRESOURCE_DATA vertex_buffer_data = { vertices.data() };
  result = device->CreateBuffer(&buffer_desc, &vertex_buffer_data, &vertex_buffer);
  Utils::checkHresult(result, "Could not create the vertex buffer!");

  // Similarly, set up the indices buffer
  ZeroMemory(&buffer_desc, sizeof(buffer_desc));
  buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                       // write access access by CPU and read access by GPU
  buffer_desc.ByteWidth = sizeof(unsigned int) * indices.size(); // Size of the buffer we want to allocate
  buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;               // use as an index buffer
  buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;           // allow CPU to write in buffer
  buffer_desc.MiscFlags = 0;

  // Create the buffer and copy the indices into it
  D3D11_SUBRESOURCE_DATA index_buffer_data = { indices.data() };
  result = device->CreateBuffer(&buffer_desc, &index_buffer_data, &index_buffer);
  Utils::checkHresult(result, "Could not create the vertex buffer!");

  // Create the bounding box for this mesh
  DirectX::BoundingOrientedBox::CreateFromPoints(bounding_box, vertex_count, vertex_positions, sizeof(DirectX::XMFLOAT3));
  DirectX::XMFLOAT3 corners[bounding_box.CORNER_COUNT];
  bounding_box.GetCorners(corners);
  std::vector<vertex_t> bounding_box_vertices;
  std::vector<unsigned int> bounding_box_indices = {
    0, 1, 2, 3, 0, 3, 1, 2,
    4, 5, 6, 7, 4, 7, 5, 6,
    0, 4, 1, 5, 2, 6, 3, 7
  };

  // Create vertices from the corners of the bounding box
  for (int i = 0; i < bounding_box.CORNER_COUNT; i++) {
    bounding_box_vertices.push_back({ corners[i], { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } });
  }

  // Setup the vertex buffer
  D3D11_BUFFER_DESC bounding_box_buffer_desc;
  ZeroMemory(&bounding_box_buffer_desc, sizeof(bounding_box_buffer_desc));

  bounding_box_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                    // write access access by CPU and read access by GPU
  bounding_box_buffer_desc.ByteWidth = sizeof(vertex_t) * bounding_box_vertices.size(); // Size of the buffer we want to allocate
  bounding_box_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;           // use as a vertex buffer
  bounding_box_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;        // allow CPU to write in buffer

  // Create the buffer and copy the vertices into it
  D3D11_SUBRESOURCE_DATA bounding_box_vertex_buffer_data = { bounding_box_vertices.data() };
  result = device->CreateBuffer(&bounding_box_buffer_desc, &bounding_box_vertex_buffer_data, &bounding_box_vertex_buffer);
  Utils::checkHresult(result, "Could not create the bounding box vertex buffer!");

  // Similarly, set up the indices buffer
  ZeroMemory(&bounding_box_buffer_desc, sizeof(bounding_box_buffer_desc));
  bounding_box_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                       // write access access by CPU and read access by GPU
  bounding_box_buffer_desc.ByteWidth = sizeof(unsigned int) * bounding_box_indices.size(); // Size of the buffer we want to allocate
  bounding_box_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;               // use as an index buffer
  bounding_box_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;           // allow CPU to write in buffer
  bounding_box_buffer_desc.MiscFlags = 0;

  // Create the buffer and copy the indices into it
  D3D11_SUBRESOURCE_DATA bounding_box_index_buffer_data = { bounding_box_indices.data() };
  result = device->CreateBuffer(&bounding_box_buffer_desc, &bounding_box_index_buffer_data, &bounding_box_index_buffer);
  Utils::checkHresult(result, "Could not create the bounding box vertex buffer!");
}

void Mesh::render() {
  // Set vertex and index buffers on the GPU to be the ones of this mesh
  UINT stride = sizeof(vertex_t);
  UINT offset = 0;
  device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
  device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

  // Set the texture if it exists, otherwise remove any previously added textures
  if (p_texture_view) {
    device_context->PSSetShaderResources(0, 1, &p_texture_view);
  }
  else {
    ID3D11ShaderResourceView *nulltexture = NULL;
    device_context->PSSetShaderResources(0, 1, &nulltexture);
  }

  // We'll be rendering a triangle list, so we need to tell the GPU
  // to render the vertices as such.
  device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // And finally, we can call the actual draw method, which will draw all the
  // indices of this mesh.
  device_context->DrawIndexed(index_count, 0, 0);

  // Next, draw the bounding box
  device_context->IASetVertexBuffers(0, 1, &bounding_box_vertex_buffer, &stride, &offset);
  device_context->IASetIndexBuffer(bounding_box_index_buffer, DXGI_FORMAT_R32_UINT, 0);
  ID3D11ShaderResourceView *nulltexture = NULL;
  device_context->PSSetShaderResources(0, 1, &nulltexture);
  device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
  device_context->DrawIndexed(24, 0, 0);
}

void Mesh::registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  Mesh::device = device;
  Mesh::device_context = device_context;
}
