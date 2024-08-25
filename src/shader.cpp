#include <xre/shader.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Shader::Shader() {}

//------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------
Shader::Shader(const char *shader_path) {
  std::string obj_location = Utils::getFileLocation(shader_path);
  std::wstring w_obj_location = Utils::stringToWString(obj_location);

  // Load and compile the pixel and vertex shaders
  ID3DBlob *vertex_shader_blob = nullptr;
  ID3DBlob *pixel_shader_blob = nullptr;
  ID3DBlob *errors_blob = nullptr;
  D3DCompileFromFile(w_obj_location.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VShader", "vs_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vertex_shader_blob, &errors_blob);

  if (errors_blob) {
    std::cout << reinterpret_cast<const char *>(errors_blob->GetBufferPointer()) << std::endl;
    Utils::exitWithMessage("The vertex shader failed to compile.");
  }

  D3DCompileFromFile(w_obj_location.c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PShader", "ps_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &pixel_shader_blob, &errors_blob);

  if (errors_blob) {
    std::cout << reinterpret_cast<const char *>(errors_blob->GetBufferPointer()) << std::endl;
    Utils::exitWithMessage("The pixel shader failed to compile.");
  }

  // Encapsulate both shaders into shader objects
  s_device->CreateVertexShader(vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), NULL, &m_vertex_shader);
  s_device->CreatePixelShader(pixel_shader_blob->GetBufferPointer(), pixel_shader_blob->GetBufferSize(), NULL, &m_pixel_shader);

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
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };

  // Create input layout
  s_device->CreateInputLayout(input_element_description, _countof(input_element_description), vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &m_input_layout);

  // Set input layout
  s_device_context->IASetInputLayout(m_input_layout);

  // Create constant buffers for our shaders
  HRESULT result;

  // Create the per model constant buffer
  D3D11_BUFFER_DESC const_buffer_desc;
  ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));

  // Setup the const buffer description. The per model const buffer has 2 4x4 matrices
  // plus a 4 value vector
  const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  const_buffer_desc.ByteWidth = 144;
  const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  result = s_device->CreateBuffer(&const_buffer_desc, NULL, &m_p_per_model_const_buffer);
  Utils::checkHresult(result, "Could not create the per model const buffer");

  // Initialize the two matrices for the per model matrix to identity matrices
  m_per_model_const_buffer.model = DirectX::XMMatrixIdentity();
  m_per_model_const_buffer.normal_rotation = DirectX::XMMatrixIdentity();

  // Initialize with red color for now
  m_per_model_const_buffer.color = { 1.0f, 0.0f, 0.0f, 1.0f };
}

Shader Shader::loadOrCreate(const char *shader_path) {
  // Check if the shader is in the map
  if(s_shader_instances.find(shader_path) == s_shader_instances.end()) {
    Shader new_shader = Shader(shader_path);
    s_shader_instances[shader_path] = new_shader;
    return new_shader;
  }
  else {
    return s_shader_instances[shader_path];
  }
}

void Shader::createGlobalBuffers(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  HRESULT result;

  // Create the per frame constant buffer
  D3D11_BUFFER_DESC const_buffer_desc;
  ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));

  // Setup the const buffer description. The per frame const buffer has 1 4x4 matrix
  const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  const_buffer_desc.ByteWidth = 64;
  const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  result = device->CreateBuffer(&const_buffer_desc, NULL, &s_p_global_per_frame_const_buffer);
  Utils::checkHresult(result, "Could not create the per frame const buffer");

  // Set a default value for the view projection matrix
  s_global_per_frame_const_buffer.view_projection = DirectX::XMMatrixIdentity();

  // Set the per frame constant buffer to slot 0
  device_context->VSSetConstantBuffers(0, 1, &s_p_global_per_frame_const_buffer);
  device_context->UpdateSubresource(s_p_global_per_frame_const_buffer, 0, 0, &s_global_per_frame_const_buffer, 0, 0);


  // Create the lighting buffer
  ZeroMemory(&const_buffer_desc, sizeof(const_buffer_desc));

  const_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  const_buffer_desc.ByteWidth = 48;
  const_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

  result = device->CreateBuffer(&const_buffer_desc, NULL, &s_p_lighting_const_buffer);
  Utils::checkHresult(result, "Could not create the lighting const buffer");

  // Initialize the color values to some sensible defaults
  s_lighting_const_buffer.ambient_color = { 0.2f, 0.2f, 0.2f, 0.3f };
  s_lighting_const_buffer.light_vector = { 1.0f, 1.0f, 1.0f, 0.0f };
  s_lighting_const_buffer.light_color = { 0.5f, 0.5f, 0.5f, 1.0f };

  // Light buffer is in slot 2 as otherwise we would need to pad it to 16 bytes (if we'd want to put
  // it before the model buffer).
  device_context->VSSetConstantBuffers(2, 1, &s_p_lighting_const_buffer);
  device_context->UpdateSubresource(s_p_lighting_const_buffer, 0, 0, &s_lighting_const_buffer, 0, 0);
}

void Shader::updateViewProjectionMatrix(DirectX::XMMATRIX view_projection) {
  s_global_per_frame_const_buffer.view_projection = view_projection;
  s_device_context->UpdateSubresource(s_p_global_per_frame_const_buffer, 0, 0, &s_global_per_frame_const_buffer, 0, 0);
}

void Shader::activate() {
  // Set both shaders to be active but only if the current
  // active shader is not already the present one
  if (Shader::s_current_active_shader == this) {
    return;
  }

  // Tell the GPU to use this shader
  s_device_context->VSSetShader(m_vertex_shader, 0, 0);
  s_device_context->PSSetShader(m_pixel_shader, 0, 0);

  // Also make sure we're using the correct constant buffer
  s_device_context->VSSetConstantBuffers(1, 1, &m_p_per_model_const_buffer);

  // And keep track of current set shader
  Shader::s_current_active_shader = this;
}

void Shader::updatePerModelConstantBuffer() {
  s_device_context->UpdateSubresource(m_p_per_model_const_buffer, 0, 0, &m_per_model_const_buffer, 0, 0);
}

void Shader::cleanUp() {
  m_p_per_model_const_buffer->Release();
  m_input_layout->Release();
  m_vertex_shader->Release();
  m_pixel_shader->Release();
}

void Shader::setModelMatrix(DirectX::XMMATRIX model_matrix) {
  m_per_model_const_buffer.model = model_matrix;
}

void Shader::setNormalRotationMatrix(DirectX::XMMATRIX rotation_matrix) {
  m_per_model_const_buffer.normal_rotation = rotation_matrix;
}

void Shader::setModelColor(DirectX::XMFLOAT4 color) {
  m_per_model_const_buffer.color = color;
}

void Shader::registerDx11DeviceAndDeviceContext(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  Shader::s_device = device;
  Shader::s_device_context = device_context;
}
