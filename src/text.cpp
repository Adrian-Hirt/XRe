#include <xre/text.h>

Text::Text(const std::string sentence, std::shared_ptr<VulkanHandler> vulkan_handler, bool stick_to_hud) {
  m_vulkan_handler = vulkan_handler;
  m_stick_to_hud = stick_to_hud;
  buildMeshesFromSentence(sentence);
}

TextChar Text::computeTextureOffsets(int letter) {
  int row = letter / 32;
  int column = letter % 32;

  float left = column * X_STEP;
  float right = left + X_STEP;
  float top = row * Y_STEP;
  float bottom = top + Y_STEP;

  TextChar character;
  character.left = left;
  character.right = right;
  character.top = top;
  character.bottom = bottom;

  return character;
}

void Text::buildMeshesFromSentence(const std::string sentence) {
  int lenght = sentence.length();
  float x_offset = 0.0f;
  float char_height = 0.1;
  float char_width = 0.05f;

  glm::vec3 normal = {0.0f, 0.0f, 1.0f};

  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
  uint16_t current_index = 0;

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
      TextChar text_character = computeTextureOffsets(letter);

      // clang-format off
      // Build vertices, for now with a "default" size and position
      vertices.push_back({glm::vec3(x_offset,              0.0f, 0.0f),               normal, glm::vec2(text_character.left, text_character.top)});
      vertices.push_back({glm::vec3(x_offset + char_width, 0.0f, 0.0f),               normal, glm::vec2(text_character.right, text_character.top)});
      vertices.push_back({glm::vec3(x_offset + char_width, 0.0f - char_height, 0.0f), normal, glm::vec2(text_character.right, text_character.bottom)});
      vertices.push_back({glm::vec3(x_offset,              0.0f - char_height, 0.0f), normal, glm::vec2(text_character.left, text_character.bottom)});
      // clang-format on

      x_offset += char_width;

      // Add indices
      for (uint16_t offset : {0, 1, 2, 3, 0, 2}) {
        indices.push_back(current_index + offset);
      }

      // Increase index counter
      current_index += 4;
    }
  }

  // Create the mesh
  Mesh mesh = Mesh(vertices, indices, m_vulkan_handler);
  std::vector<Mesh> meshes = {mesh};

  // Create the texture and the material
  std::shared_ptr<Texture> texture = std::make_shared<Texture>(DATA_FOLDER "fonts/DejaVuSansMono128NoAA.png", m_vulkan_handler);

  std::string vertex_shader;
  if (m_stick_to_hud) {
    vertex_shader = SHADERS_FOLDER "bitmap.vert.spv";
  } else {
    vertex_shader = SHADERS_FOLDER "basic.vert.spv";
  }

  std::shared_ptr<Material> material =
      std::make_shared<Material>(vertex_shader, SHADERS_FOLDER "texture.frag.spv", texture, false, m_vulkan_handler);

  // Create the model
  m_model = std::make_shared<Model>(meshes, glm::vec3(1.0f, 0.0f, 0.0f), material);
}

std::shared_ptr<Model> Text::getModel() { return m_model; }
