layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
  mat4 view_projection;
} globalUBO;

layout(set = 1, binding = 0) uniform ModelUniformBufferObject {
  mat4 world;
} modelUBO;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position; // In world space

void main() {
  vec4 pos = modelUBO.world * vec4(inPosition, 1.0);
  gl_Position = globalUBO.view_projection * pos;
  color = inColor;
  position = pos.xyz;
}