#include <xre/mesh.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Mesh::Mesh() {};

//------------------------------------------------------------------------------------------------------
// Initialize the mesh.
// Arguments:
//  1) DirectX Device
//  2) DirectX DeviceContext
//  3) Vector with the vertices of the mesh
//  4) Vector with the incides of the mesh
//------------------------------------------------------------------------------------------------------
Mesh::Mesh(ID3D11Device *device, ID3D11DeviceContext *device_context, std::vector<vertex> vertices, std::vector<unsigned int> indices) {
  // Set the data we got passed in
  this->device = device;
  this->device_context = device_context;

  // Setup the mesh
  HRESULT result;

  // Store number of vertices and indices
  vertex_count = vertices.size();
  index_count = indices.size();

  // Setup the vertex buffer
  D3D11_BUFFER_DESC buffer_desc;
  ZeroMemory(&buffer_desc, sizeof(buffer_desc));

  buffer_desc.Usage = D3D11_USAGE_DYNAMIC;                    // write access access by CPU and read access by GPU
  buffer_desc.ByteWidth = sizeof(vertex) * vertices.size(); // Size of the buffer we want to allocate
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
  UINT stride = sizeof(vertex);
  UINT offset = 0;
  device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
  device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

  // We'll be rendering a triangle list, so we need to tell the GPU
  // to render the vertices as such.
  device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // And finally, we can call the actual draw method, which will draw all the
  // indices of this mesh.
  device_context->DrawIndexed(index_count, 0, 0);
}

void Mesh::render(Shader shader) {
  // Activate the shader
  shader.activate();

  // Use the normal render function
  render();
}
