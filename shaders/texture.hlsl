#include "_common.hlsl"

Texture2D Texture;
SamplerState Sampler;

VOut VShader(VIn input) {
  VOut output;

  // Calculate the position
  output.position = mul(mul(input.position, model), view_projection);

  // Simply use white as the color for now, as we're discarding
  // the color anyway
	output.color = color_white;

  // Set the texture coordinates
  output.texcoord = input.texcoord;

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET {
  float4 result = Texture.Sample(Sampler, texcoord);

  return result;
}
