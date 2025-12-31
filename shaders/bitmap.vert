#version 450 core
#include "_common.vert"

void main() {
  // Transform unit quad by model matrix
  vec4 pos = modelUBO.world * vec4(inPosition, 1.0);

  // Map pixel-space coordinates to normalized clip space (-1..1)
  vec2 clipXY = pos.xy * 2.0 - 1.0;
  clipXY.y = -clipXY.y;
  gl_Position = vec4(clipXY, pos.z, 1.0);

  // Set color to color of the model
  color = modelUBO.color;

  // Set fragment texture coordinates
  fragTexCoord = inTexCoord;
}
