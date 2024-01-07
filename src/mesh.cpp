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
  HRESULT result = DirectX::CreateWICTextureFromFile(device, device_context, filepath.c_str(), &p_texture, &p_texture_view);
  Utils::checkHresult(result, "Failed to load the texture"); // TODO: output the filename that was not found
}

// Function to initialize the "common" data of a mesh, to avoid code-duplication
void Mesh::initialize(std::vector<vertex_t> vertices, std::vector<unsigned int> indices) {
  // Setup the mesh
  HRESULT result;

  // Store number of vertices and indices
  vertex_count = vertices.size();
  index_count = indices.size();

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
}

void Mesh::registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  Mesh::device = device;
  Mesh::device_context = device_context;
}
