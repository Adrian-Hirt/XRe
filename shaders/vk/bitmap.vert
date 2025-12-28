#version 450 core

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
  mat4 view_projection;
  vec3 light_vector;
  vec3 light_color;
  vec3 ambient_color;
} globalUBO;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
  mat4 world;
  vec3 color;
} modelUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 color;
layout(location = 1) out vec2 fragTexCoord;

void main() {
  // Transform unit quad by model matrix
  vec4 pos = modelUBO.world * vec4(inPosition, 1.0);

  // Map pixel-space coordinates to normalized clip space (-1..1)
  vec2 clipXY = pos.xy * 2.0 - 1.0;
  clipXY.y = -clipXY.y;
  gl_Position = vec4(clipXY, pos.z, 1.0);

  // Set color to color of the model
  color = vec3(inTexCoord, 0.0);

  // Set fragment texture coordinates
  fragTexCoord = inTexCoord;
}
