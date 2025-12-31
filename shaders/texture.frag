#version 450 core
#include "_common.frag"

layout(set = 1, binding = 1) uniform sampler2D texSampler;

void main() {
  outColor = texture(texSampler, fragmentCoordinate);
}
