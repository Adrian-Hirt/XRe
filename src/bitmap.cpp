#include <xre/bitmap.h>

Bitmap::Bitmap(float top_left_position_x, float top_left_position_y, float x_extend, float y_extend, const char *texture_path) {
  DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f};
  DirectX::XMFLOAT3 normal = { 0.0f, 0.0f, 1.0f };

  std::vector<vertex_t> vertices = {
    {  DirectX::XMFLOAT3(top_left_position_x,            top_left_position_y,            0.0f), color, normal, DirectX::XMFLOAT2(1.0f, 0.0f) },
    {  DirectX::XMFLOAT3(top_left_position_x + x_extend, top_left_position_y,            0.0f), color, normal, DirectX::XMFLOAT2(0.0f, 0.0f) },
    {  DirectX::XMFLOAT3(top_left_position_x + x_extend, top_left_position_y - y_extend, 0.0f), color, normal, DirectX::XMFLOAT2(0.0f, 1.0f) },
    {  DirectX::XMFLOAT3(top_left_position_x,            top_left_position_y - y_extend, 0.0f), color, normal, DirectX::XMFLOAT2(1.0f, 1.0f) }
  };

  std::vector<unsigned int> indices = {0, 1, 2, 3, 0, 2};

  // Call general initialize method
  initialize(vertices, indices);

  // Load the texture
  std::wstring filepath = Utils::stringToWString(texture_path);
  HRESULT result = DirectX::CreateWICTextureFromFile(device, device_context, filepath.c_str(), &p_texture, &p_texture_view);
  Utils::checkHresult(result, "Failed to load the texture"); // TODO: output the filename that was not found

  shader = Shader(SHADERS_FOLDER "/bitmap.hlsl");
}

Bitmap::Bitmap(std::vector<vertex_t> vertices, std::vector<unsigned int> indices, const char *texture_path) {
  // Call general initialize method
  initialize(vertices, indices);

  // Load the texture
  std::wstring filepath = Utils::stringToWString(texture_path);
  HRESULT result = DirectX::CreateWICTextureFromFile(device, device_context, filepath.c_str(), &p_texture, &p_texture_view);
  Utils::checkHresult(result, "Failed to load the texture");

  shader = Shader(SHADERS_FOLDER "/bitmap.hlsl");
}

void Bitmap::render() {
  shader.activate();
  Mesh::render();
}
