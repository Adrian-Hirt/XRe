#include <xre/shader.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Shader::Shader() {};

//------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------
Shader::Shader(const char *shader_path, ID3D11Device *device, ID3D11DeviceContext *device_context) {
  this->device = device;
  this->device_context = device_context;

  std::string obj_location = getFileLocation(shader_path);
  std::wstring w_obj_location = stringToWString(obj_location);

  // Load and compile the pixel and vertex shaders
  ID3D10Blob *vertex_shader_blob, *pixel_shader_blob, *errors_blob;
  D3DCompileFromFile(w_obj_location.c_str(), 0, 0, "VShader", "vs_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vertex_shader_blob, &errors_blob);
  if (errors_blob) {
    std::cout << reinterpret_cast<const char *>(errors_blob->GetBufferPointer()) << std::endl;
    exit_with_message("The vertex shader failed to compile.");
  }

  D3DCompileFromFile(w_obj_location.c_str(), 0, 0, "PShader", "ps_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &pixel_shader_blob, &errors_blob);

  if (errors_blob) {
    std::cout << reinterpret_cast<const char *>(errors_blob->GetBufferPointer()) << std::endl;
    exit_with_message("The pixel shader failed to compile.");
  }

  // Encapsulate both shaders into shader objects
  device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), NULL, &vertex_shader);
  device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), NULL, &pixel_shader);

  // D3D11_INPUT_ELEMENT_DESC:
  //  1. Semantic => Describes what the data is used for, e.g. "COLOR"
  //  2. Semantic index (if we have multiple same semantics, e.g. multiple colors)
  //  3. Format of the data  (needs to match what we have in the data)
  //  4. Input slot, leave at 0 for now
  //  5. Offset in bytes in the vertex data struct (may also use `D3D11_INPUT_ELEMENT_DESC` to align directly after previous element)
  //  6. Input slot class, the data we have is per vertex
  //  7. Mut be zero when using per vertex data
  D3D11_INPUT_ELEMENT_DESC input_element_description[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  // Create input layout
  device->CreateInputLayout(input_element_description, 2, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);

  // Set input layout
  device_context->IASetInputLayout(input_layout);
};

void Shader::render() {
  // Activate the shader, as another shader might have been set to active
  this->activate();

  // select which vertex buffer to display
  UINT stride = sizeof(vertex);
  UINT offset = 0;
  this->device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

  // select which primtive type we are using
  this->device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // draw the vertex buffer to the back buffer
  this->device_context->Draw(3, 0);
}

void Shader::activate() {
  // Set both shaders to be active but only if the current
  // active shader is not already the present one
  if (Shader::current_active_shader == this) {
    return;
  }

  // Tell the GPU to use this shader
  this->device_context->VSSetShader(vertex_shader, 0, 0);
  this->device_context->PSSetShader(pixel_shader, 0, 0);

  // And keep track of current set shader
  Shader::current_active_shader = this;
}

void Shader::setVertexData(vertex vertex_data[], size_t vertex_data_size) {
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  bd.Usage = D3D11_USAGE_DYNAMIC;             // write access access by CPU and read access by GPU
  bd.ByteWidth = vertex_data_size;            // Size of the buffer we want to allocate
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // use as a vertex buffer
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // allow CPU to write in buffer

  device->CreateBuffer(&bd, NULL, &vertex_buffer); // create the buffer

  D3D11_MAPPED_SUBRESOURCE ms;
  device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms); // map the buffer
  memcpy(ms.pData, vertex_data, vertex_data_size);                        // copy the data
  device_context->Unmap(vertex_buffer, 0);                                // unmap the buffer
}

void Shader::cleanUp() {
  input_layout->Release();
  vertex_shader->Release();
  pixel_shader->Release();
  vertex_buffer->Release();
}
