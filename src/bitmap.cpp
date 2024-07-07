#include <xre/bitmap.h>

Bitmap::Bitmap() {}

Bitmap::Bitmap(float top_left_position_x, float top_left_position_y, float x_extend, float y_extend, const char *texture_path) {
  DirectX::XMFLOAT3 normal = { 0.0f, 0.0f, 1.0f };

  std::vector<vertex_t> vertices = {
    {  DirectX::XMFLOAT3(top_left_position_x,            top_left_position_y,            0.0f), normal, DirectX::XMFLOAT2(1.0f, 0.0f) },
    {  DirectX::XMFLOAT3(top_left_position_x + x_extend, top_left_position_y,            0.0f), normal, DirectX::XMFLOAT2(0.0f, 0.0f) },
    {  DirectX::XMFLOAT3(top_left_position_x + x_extend, top_left_position_y - y_extend, 0.0f), normal, DirectX::XMFLOAT2(0.0f, 1.0f) },
    {  DirectX::XMFLOAT3(top_left_position_x,            top_left_position_y - y_extend, 0.0f), normal, DirectX::XMFLOAT2(1.0f, 1.0f) }
  };

  std::vector<unsigned int> indices = {0, 1, 2, 3, 0, 2};

  // Set color to white, which will be discarded anyway
  m_bitmap_color = { 1.0f, 1.0f, 1.0f, 1.0f };

  // Call general initialize method
  initialize(vertices, indices);

  // Load the texture
  std::wstring filepath = Utils::stringToWString(texture_path);
  ID3D11Resource *texture; // Throwaway variable
  HRESULT result = DirectX::CreateWICTextureFromFile(m_device, m_device_context, filepath.c_str(), &texture, &m_texture_view);
  std::string error_string = "Failed to load the texture ";
  error_string += texture_path;
  Utils::checkHresult(result, error_string.c_str());

  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/bitmap.hlsl");
}

Bitmap::Bitmap(std::vector<vertex_t> vertices, std::vector<unsigned int> indices, const char *texture_path) {
  // Call general initialize method
  initialize(vertices, indices);

  // Load the texture
  std::wstring filepath = Utils::stringToWString(texture_path);
  ID3D11Resource *texture; // Throwaway variable
  HRESULT result = DirectX::CreateWICTextureFromFile(m_device, m_device_context, filepath.c_str(), &texture, &m_texture_view);
  Utils::checkHresult(result, "Failed to load the texture");

  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/bitmap.hlsl");
}

void Bitmap::render() {
  m_shader.activate();
  m_shader.setModelColor(m_bitmap_color);
  m_shader.updatePerModelConstantBuffer();
  Mesh::render();
}
