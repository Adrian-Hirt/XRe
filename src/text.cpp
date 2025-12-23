#include <xre/text.h>

text_char_t Text::computeTextureOffsets(int letter) {
  int row = letter / 32;
  int column = letter % 32;

  float left = column * X_STEP;
  float right = left + X_STEP;
  float top = row * Y_STEP;
  float bottom = top + Y_STEP;

  text_char_t character;
  character.left = left;
  character.right = right;
  character.top = top;
  character.bottom = bottom;

  return character;
}

Text::Text(const char *sentence) { buildMeshesFromSentence(sentence); }

void Text::buildMeshesFromSentence(const char *sentence) {
  int lenght = strlen(sentence);

  float x_offset = -0.95f;
  float char_height = 0.1;
  float char_width = 0.05f;

  DirectX::XMFLOAT3 normal = {0.0f, 0.0f, 1.0f};

  std::vector<vertex_t> vertices;
  std::vector<unsigned int> indices;

  unsigned int current_index = 0;

  for (int i = 0; i < lenght; i++) {
    int letter = ((int)sentence[i]) - 32;

    if (letter < 0 || letter > CHAR_COUNT) {
      // Invalid character, put a question mark
      letter = 31;
    }

    if (letter == 0) {
      // Space, just create a space between the meshes
      x_offset += char_width;
    } else {
      text_char_t text_character = computeTextureOffsets(letter);

      // Build vertices, for now with a "default" size and position
      vertices.push_back({DirectX::XMFLOAT3(x_offset, 0.95f, 0.0f), normal, DirectX::XMFLOAT2(text_character.left, text_character.top)});
      vertices.push_back(
          {DirectX::XMFLOAT3(x_offset + char_width, 0.95f, 0.0f), normal, DirectX::XMFLOAT2(text_character.right, text_character.top)});
      vertices.push_back({DirectX::XMFLOAT3(x_offset + char_width, 0.95f - char_height, 0.0f), normal,
                          DirectX::XMFLOAT2(text_character.right, text_character.bottom)});
      vertices.push_back(
          {DirectX::XMFLOAT3(x_offset, 0.95f - char_height, 0.0f), normal, DirectX::XMFLOAT2(text_character.left, text_character.bottom)});

      x_offset += char_width;

      // Add indices
      indices.insert(indices.end(),
                     {current_index, current_index + 1, current_index + 2, current_index + 3, current_index, current_index + 2});

      // Increase index counter
      current_index += 4;
    }
  }

  // Call general initialize method
  initialize(vertices, indices);

  // Load the texture
  std::wstring filepath = Utils::stringToWString(DATA_FOLDER "/fonts/DejaVuSansMono128NoAA.png");
  ID3D11Resource *texture; // Throwaway variable
  HRESULT result = DirectX::CreateWICTextureFromFile(s_device, s_device_context, filepath.c_str(), &texture, &m_texture_view);
  Utils::checkHresult(result, "Failed to load the texture");

  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/bitmap.hlsl");
}

void Text::render() {
  m_shader.activate();
  m_shader.updatePerModelConstantBuffer();

  Renderable::render();
}
