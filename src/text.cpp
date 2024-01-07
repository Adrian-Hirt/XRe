#include <xre/text.h>

Text::Text() {
  // Using the extendes ASCII charset for now, which has 255 characters
  font = new text_char_t[255];

  // Grid is 16x16
  float step = 1.0f / 16.0f;
  float halfstep = step / 2;

  for(int i = 0; i < 255; i++) {
    int row = i / 16;
    int column = i % 16;

    float left = column * step;
    float right = left + halfstep;
    float top = row * step;
    float bottom = top + step;

    font[i].left = left;
    font[i].right = right;
    font[i].top = top;
    font[i].bottom = bottom;
  }
}

void Text::buildMeshesFromSentence(const char* sentence) {
  int lenght = strlen(sentence);

  float x_position = 0.0;
  float y_position = 0.0;

  float x_offset = -0.95f;
  float char_height = 0.1;
  float char_width = 0.05f;

  DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f};
  DirectX::XMFLOAT3 normal = { 0.0f, 0.0f, 1.0f };

  for (int i = 0; i < lenght; i++) {
    int letter = ((int) sentence[i]) - 32;

    if (letter == 0) {
      x_offset += char_width;
    }
    else {
      text_char_t text_character = font[letter];

      // Build vertices, for now with a "default" size and position
      std::vector<vertex_t> vertices = {
        {  DirectX::XMFLOAT3(x_offset,              0.95f,               0.0f), color, normal, DirectX::XMFLOAT2(text_character.left, text_character.top) },
        {  DirectX::XMFLOAT3(x_offset + char_width, 0.95f,               0.0f), color, normal, DirectX::XMFLOAT2(text_character.right, text_character.top) },
        {  DirectX::XMFLOAT3(x_offset + char_width, 0.95f - char_height, 0.0f), color, normal, DirectX::XMFLOAT2(text_character.right, text_character.bottom) },
        {  DirectX::XMFLOAT3(x_offset,              0.95f - char_height, 0.0f), color, normal, DirectX::XMFLOAT2(text_character.left, text_character.bottom) }
      };

      x_offset += char_width;

      std::vector<unsigned int> indices = {0, 1, 2, 3, 0, 2};

      bitmaps.push_back(Bitmap(vertices, indices, DATA_FOLDER "/fonts/DejaVuSansMono32.png"));
    }
  }
}

void Text::render() {
  for (Bitmap bitmap : bitmaps) {
    bitmap.render();
  }
}
