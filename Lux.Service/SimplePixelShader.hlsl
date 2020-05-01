Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);

struct input_t
{
  float4 Screen : SV_POSITION;
  float2 Texture : TEXCOORD0;
};

float4 main(input_t input) : SV_TARGET
{
  return Texture.Sample(Sampler, input.Texture);
}