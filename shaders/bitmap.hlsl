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

struct VOut {
  float4 position : SV_POSITION;
  float4 color : COLOR;
  float2 texcoord : TEXCOORD;
};
Texture2D Texture;
SamplerState Sampler;

float InverseGammaCorrect(float value) {
  if (value <= 0.04045) {
    return value * 1.0 / 12.92;
  }

  return pow(abs((value + 0.0550) * 1.0 / 1.055), 2.4f);
}

VOut VShader(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD) {
  VOut output;

  // Set the position to the incoming position as we're not doing any projection
  output.position =position;

  // Simply white as the color for now
  float4 white = { 1.0f, 1.0f, 1.0f, 1.0f };
	output.color = white;

  output.texcoord = texcoord;

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET {
  float4 result = Texture.Sample(Sampler, texcoord);

  result.r = InverseGammaCorrect(result.r);
  result.g = InverseGammaCorrect(result.g);
  result.b = InverseGammaCorrect(result.b);

  return result;
}
