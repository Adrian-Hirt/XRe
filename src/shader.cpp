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

  std::string obj_location = Utils::getFileLocation(shader_path);
  std::wstring w_obj_location = Utils::stringToWString(obj_location);

  // Load and compile the pixel and vertex shaders
  ID3D10Blob *vertex_shader_blob, *pixel_shader_blob, *errors_blob;
  D3DCompileFromFile(w_obj_location.c_str(), 0, 0, "VShader", "vs_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vertex_shader_blob, &errors_blob);
  if (errors_blob) {
    std::cout << reinterpret_cast<const char *>(errors_blob->GetBufferPointer()) << std::endl;
    Utils::exitWithMessage("The vertex shader failed to compile.");
  }

  D3DCompileFromFile(w_obj_location.c_str(), 0, 0, "PShader", "ps_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &pixel_shader_blob, &errors_blob);

  if (errors_blob) {
    std::cout << reinterpret_cast<const char *>(errors_blob->GetBufferPointer()) << std::endl;
    Utils::exitWithMessage("The pixel shader failed to compile.");
  }

  // Encapsulate both shaders into shader objects
  device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), NULL, &vertex_shader);
  device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), NULL, &pixel_shader);

  // D3D11_INPUT_ELEMENT_DESC:
  //  1. Semantic => Describes what the data is used for, e.g. "COLOR"
  //  2. Semantic index (if we have multiple same semantics, e.g. multiple colors)
  //  3. Format of the data  (needs to match what we have in the data)
  //  4. Input slot, leave at 0 for now
  //  5. Offset in bytes in the vertex data struct (may also use `D3D11_APPEND_ALIGNED_ELEMENT` to align directly after previous element)
  //  6. Input slot class, the data we have is per vertex
  //  7. Mut be zero when using per vertex data
  D3D11_INPUT_ELEMENT_DESC input_element_description[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };

  // Create input layout
  device->CreateInputLayout(input_element_description, 3, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);

  // Set input layout
  device_context->IASetInputLayout(input_layout);

  // Create constant buffers for our shaders
  D3D11_BUFFER_DESC const_buffer_desc;
  ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));

  // Setup the const buffer description. Our constant buffer will have 3 4x4 float matrices (model, view_projection & normal_rotation
  // matrix) and one 3-float values. 1 Float is 4 bytes, i.e. we have 3 (Matrices) * 16 (Values per matrix) * 4 (Bytes per value)
  // + 3 (vectors) * 4 (values per vector) * 4 (Bytes per value) = 240 bytes
  const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  const_buffer_desc.ByteWidth = 240;
  const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  HRESULT result;

  result = device->CreateBuffer(&const_buffer_desc, NULL, &p_const_buffer);
  Utils::checkHresult(result, "Could not create the const buffer");

  // Initialize the constant buffer values to use identity matrices for all matrix values.
  // This allows us to use the shader directly, without having to manually set the
  // values before rendering.
  const_buffer.model = DirectX::XMMatrixIdentity();
  const_buffer.view_projection = DirectX::XMMatrixIdentity();
  const_buffer.normal_rotation = DirectX::XMMatrixIdentity();

  // Initialize the color values to some sensible defaults
  const_buffer.ambient_color = { 0.2f, 0.2f, 0.2f, 0.3f };
  const_buffer.light_vector = { 1.0f, 1.0f, 1.0f, 0.0f };
  const_buffer.light_color = { 0.5f, 0.5f, 0.5f, 1.0f };

  // Send the values to the const buffer
  updateConstantBuffer();
};

void Shader::activate() {
  // Set both shaders to be active but only if the current
  // active shader is not already the present one
  if (Shader::current_active_shader == this) {
    return;
  }

  // Tell the GPU to use this shader
  this->device_context->VSSetShader(vertex_shader, 0, 0);
  this->device_context->PSSetShader(pixel_shader, 0, 0);

  // Also make sure we're using the correct constant buffer
  this->device_context->VSSetConstantBuffers(0, 1, &p_const_buffer);

  // And keep track of current set shader
  Shader::current_active_shader = this;
}

void Shader::updateConstantBuffer() {
  // Update the constant buffer with the viewProjection matrix
  const_buffer.view_projection = view_projection;

  device_context->UpdateSubresource(p_const_buffer, 0, 0, &const_buffer, 0, 0);
}

void Shader::cleanUp() {
  p_const_buffer->Release();
  input_layout->Release();
  vertex_shader->Release();
  pixel_shader->Release();
}

void Shader::setViewProjectionMatrix(DirectX::XMMATRIX view_projection) {
  // Update the view_projection static variable, as this
  // is shared between all shaders
  Shader::view_projection = view_projection;
}

void Shader::setModelMatrix(DirectX::XMMATRIX model_matrix) {
  const_buffer.model = model_matrix;
}

void Shader::setNormalRotationMatrix(DirectX::XMMATRIX rotation_matrix) {
  const_buffer.normal_rotation = rotation_matrix;
}
