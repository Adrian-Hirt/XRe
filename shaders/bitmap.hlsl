#include "_common.hlsl"

Texture2D Texture;
SamplerState Sampler;

VOut VShader(VIn input) {
  VOut output;

  // Set the position to the incoming position as we're not doing any projection
  output.position = input.position;

  // Simply use white as the color for now
	output.color = color_white;

  // Set texture coordinates
  output.texcoord = input.texcoord;

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET {
  float4 result = Texture.Sample(Sampler, texcoord);

  return result;
}
