#include "_common.hlsl"

Texture2D Texture;
SamplerState Sampler;

VOut VShader(VIn input) {
  VOut output;

  // Calculate the position
  output.position = mul(mul(input.position, model), view_projection);

  // Compute color based on normal vectors
	output.color = ambient_color;
  float4 norm = normalize(mul(rotation, input.normal));
	float diffuse_brightness = saturate(dot(norm, light_vector));
  float4 resulting_light = light_color * diffuse_brightness;
  resulting_light.a = 1.0f;
	output.color += saturate(resulting_light * model_color);

  // Set texture coordinates
  output.texcoord = input.texcoord;

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET {
  float4 texture_color = Texture.Sample(Sampler, texcoord);

  return texture_color * color;
}