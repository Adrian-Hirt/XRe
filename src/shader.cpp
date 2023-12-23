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
  HRESULT result;

  // Create the per model constant buffer
  D3D11_BUFFER_DESC const_buffer_desc;
  ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));

  // Setup the const buffer description. The per model const buffer has 2 4x4 matrices
  const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  const_buffer_desc.ByteWidth = 128;
  const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  result = device->CreateBuffer(&const_buffer_desc, NULL, &p_per_model_const_buffer);
  Utils::checkHresult(result, "Could not create the per model const buffer");

  // Initialize the two matrices for the per model matrix to identity matrices
  per_model_const_buffer.model = DirectX::XMMatrixIdentity();
  per_model_const_buffer.normal_rotation = DirectX::XMMatrixIdentity();
};

void Shader::createGlobalBuffers(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  HRESULT result;

  // Create the per frame constant buffer
  D3D11_BUFFER_DESC const_buffer_desc;
  ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));

  // Setup the const buffer description. The per frame const buffer has 1 4x4 matrix
  const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  const_buffer_desc.ByteWidth = 64;
  const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  result = device->CreateBuffer(&const_buffer_desc, NULL, &p_global_per_frame_const_buffer);
  Utils::checkHresult(result, "Could not create the per frame const buffer");

  // Set a default value for the view projection matrix
  global_per_frame_const_buffer.view_projection = DirectX::XMMatrixIdentity();

  // Set the per frame constant buffer to slot 0
  device_context->VSSetConstantBuffers(0, 1, &p_global_per_frame_const_buffer);
  device_context->UpdateSubresource(p_global_per_frame_const_buffer, 0, 0, &global_per_frame_const_buffer, 0, 0);


  // Create the lighting buffer
  ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));

  const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  const_buffer_desc.ByteWidth = 48;
  const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  result = device->CreateBuffer(&const_buffer_desc, NULL, &p_lighting_const_buffer);
  Utils::checkHresult(result, "Could not create the lighting const buffer");

  // Initialize the color values to some sensible defaults
  lighting_const_buffer.ambient_color = { 0.2f, 0.2f, 0.2f, 0.3f };
  lighting_const_buffer.light_vector = { 1.0f, 1.0f, 1.0f, 0.0f };
  lighting_const_buffer.light_color = { 0.5f, 0.5f, 0.5f, 1.0f };

  // Light buffer is in slot 2 as otherwise we would need to pad it to 16 bytes (if we'd want to put
  // it before the model buffer).
  device_context->VSSetConstantBuffers(2, 1, &p_lighting_const_buffer);
  device_context->UpdateSubresource(p_lighting_const_buffer, 0, 0, &lighting_const_buffer, 0, 0);
};

void Shader::updateViewProjectionMatrix(DirectX::XMMATRIX view_projection, ID3D11DeviceContext *device_context) {
  global_per_frame_const_buffer.view_projection = view_projection;
  device_context->UpdateSubresource(p_global_per_frame_const_buffer, 0, 0, &global_per_frame_const_buffer, 0, 0);
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

  // Also make sure we're using the correct constant buffer
  this->device_context->VSSetConstantBuffers(1, 1, &p_per_model_const_buffer);

  // And keep track of current set shader
  Shader::current_active_shader = this;
}

void Shader::updatePerModelConstantBuffer() {
  device_context->UpdateSubresource(p_per_model_const_buffer, 0, 0, &per_model_const_buffer, 0, 0);
}

void Shader::cleanUp() {
  p_per_model_const_buffer->Release();
  input_layout->Release();
  vertex_shader->Release();
  pixel_shader->Release();
}

void Shader::setModelMatrix(DirectX::XMMATRIX model_matrix) {
  per_model_const_buffer.model = model_matrix;
}

void Shader::setNormalRotationMatrix(DirectX::XMMATRIX rotation_matrix) {
  per_model_const_buffer.normal_rotation = rotation_matrix;
}
