#include <xre/shader.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Shader::Shader() {};

//------------------------------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------------------------------
Shader::Shader(const char *shader_path, ID3D11Device *device, ID3D11DeviceContext *device_context) {
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

  // Set both shaders to be active
  device_context->VSSetShader(vertex_shader, 0, 0);
  device_context->PSSetShader(pixel_shader, 0, 0);

  vertex OurVertices[] = {
    {0.0f, 0.5f, 0.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
    {0.45f, -0.5, 0.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
    {-0.45f, -0.5f, 0.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}
  };

  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
  bd.ByteWidth = sizeof(vertex) * 3;             // size is the VERTEX struct * 3
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

  device->CreateBuffer(&bd, NULL, &vertex_buffer);       // create the buffer

  D3D11_MAPPED_SUBRESOURCE ms;
  device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms); // map the buffer
  memcpy(ms.pData, OurVertices, sizeof(OurVertices));                     // copy the data
  device_context->Unmap(vertex_buffer, 0);                                // unmap the buffer

  D3D11_INPUT_ELEMENT_DESC ied[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  device->CreateInputLayout(ied, 2, vertex_shader_blob->GetBufferPointer(), vertex_shader_blob->GetBufferSize(), &input_layout);
  device_context->IASetInputLayout(input_layout);
};

void Shader::render(ID3D11DeviceContext *device_context) {
  // select which vertex buffer to display
  UINT stride = sizeof(vertex);
  UINT offset = 0;
  device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

  // select which primtive type we are using
  device_context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // draw the vertex buffer to the back buffer
  device_context->Draw(3, 0);
}
