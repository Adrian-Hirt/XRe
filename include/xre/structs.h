#pragma once

// OpenXR includes
#include <open_xr/openxr.h>

// GLM
#include <glm/glm/glm.hpp>

// Other includes
#include <vector>
#include <array>
#include <optional>

typedef struct UniformBufferObject {
  glm::mat4 world;
  glm::mat4 view;
  glm::mat4 projection;
} UniformBufferObject;

typedef struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
  glm::vec2 texture_coord;

  static VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription binding_description{};
    binding_description.binding = 0;
    binding_description.stride = sizeof(Vertex);
    binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return binding_description;
  }

  static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions{};
    // Binding for position
    attribute_descriptions[0].binding = 0;
    attribute_descriptions[0].location = 0;
    attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[0].offset = offsetof(Vertex, position);

    // Binding for color
    attribute_descriptions[1].binding = 0;
    attribute_descriptions[1].location = 1;
    attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute_descriptions[1].offset = offsetof(Vertex, color);

    // Binding for texture coordinates
    attribute_descriptions[2].binding = 0;
    attribute_descriptions[2].location = 2;
    attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attribute_descriptions[2].offset = offsetof(Vertex, texture_coord);

    return attribute_descriptions;
  }

  bool operator==(const Vertex& other) const {
    return position == other.position && color == other.color && texture_coord == other.texture_coord;
  }
} Vertex;


// typedef struct per_frame_const_buffer_t {
//   DirectX::XMMATRIX view_projection;
// } per_frame_const_buffer_t;

// typedef struct lighting_const_buffer_t {
//   DirectX::XMFLOAT4 light_vector;
//   DirectX::XMFLOAT4 light_color;
//   DirectX::XMFLOAT4 ambient_color;
// } lighting_const_buffer_t;

// typedef struct per_model_const_buffer_t {
//   DirectX::XMMATRIX model;
//   DirectX::XMMATRIX normal_rotation;
//   DirectX::XMFLOAT4 color;
// } per_model_const_buffer_t;

// typedef struct text_char_t {
//   float left;
//   float right;
//   float top;
//   float bottom;
// } text_char_t;
