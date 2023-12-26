struct VOut {
  float4 position : SV_POSITION;
  float4 color : COLOR;
  float2 texcoord : TEXCOORD;
};

cbuffer PerFrameConstantBuffer{
  float4x4 view_projection;
};

cbuffer PerModelConstantBuffer{
  float4x4 model;
  float4x4 rotation;
};

cbuffer LightContstantBuffer {
  float4 light_vector;
	float4 light_color;
	float4 ambient_color;
};

Texture2D Texture;
SamplerState Sampler;

float InverseGammaCorrect(float value) {
  if (value <= 0.04045) {
    return value * 1.0 / 12.92;
  }

  return pow(abs((value + 0.0550) * 1.0 / 1.055), 2.4f);
}

VOut VShader(float4 position : POSITION, float4 color : COLOR, float4 normal : NORMAL, float2 texcoord : TEXCOORD) {
  VOut output;

  // Calculate the position
  output.position = mul(mul(position, model), view_projection);

  // Compute color based on normal vectors
	output.color = ambient_color;
  float4 norm = normalize(mul(rotation, normal));
	float diffuse_brightness = saturate(dot(norm, light_vector));
  float4 resulting_light = light_color * diffuse_brightness;
  resulting_light.a = 1.0f;
	output.color += saturate(resulting_light * color);

  output.texcoord = texcoord;

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET {
  float4 texture_color = Texture.Sample(Sampler, texcoord);

  texture_color.r = InverseGammaCorrect(texture_color.r);
  texture_color.g = InverseGammaCorrect(texture_color.g);
  texture_color.b = InverseGammaCorrect(texture_color.b);

  return texture_color * color;
}