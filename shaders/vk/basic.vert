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
layout(location = 1) out vec3 position; // In world space

void main() {
  // Transform vertex position to world space
  vec4 pos = modelUBO.world * vec4(inPosition, 1.0);
  position = pos.xyz;

  // Transform to clip space
  gl_Position = globalUBO.view_projection * pos;

  // Extract rotation+scale, then fix for non-uniform scaling
  mat3 normalMatrix = transpose(inverse(mat3(modelUBO.world)));
  vec3 norm = normalize(normalMatrix * inNormal);

  // Compute diffuse lighting
  float diffuse = clamp(dot(norm, normalize(globalUBO.light_vector)), 0.0, 1.0);
  vec3 diffuseLight = globalUBO.light_color * diffuse;

  // Combine with ambient
  color = globalUBO.ambient_color + diffuseLight;

  // And tint by general model color
  color *= modelUBO.color;
}
