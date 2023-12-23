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


VOut VShader(float4 position : POSITION, float4 color : COLOR, float4 normal : NORMAL, float2 texcoord : TEXCOORD) {
  VOut output;

  // Calculate the position
  output.position = mul(mul(position, model), view_projection);

  // Simply use the color we got passed in as the color
	output.color = color;

  output.texcoord = texcoord;

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD) : SV_TARGET {
  return Texture.Sample(Sampler, texcoord);
}