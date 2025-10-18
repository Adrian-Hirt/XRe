layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
  mat4 view_projection;
  vec3 light_vector;
  vec3 light_color;
  vec3 ambient_color;
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
  // Transform vertex position to world space
  vec4 pos = modelUBO.world * vec4(inPosition, 1.0);
  position = pos.xyz;

  // Transform to clip space
  gl_Position = globalUBO.view_projection * pos;

  // Ambient + diffuse lighting
  vec3 norm = normalize(inNormal);
  float diffuse = clamp(dot(norm, normalize(modelUBO.light_vector)), 0.0, 1.0);

  vec3 diffuseLight = modelUBO.light_color * diffuse;

  // Combine with ambient
  color = modelUBO.ambient_color + diffuseLight;

  // Optionally, you can multiply by vertex color if you want:
  color *= inColor;
}