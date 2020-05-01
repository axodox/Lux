struct input_t
{
  float3 Position : POSITION0;
  float2 Texture : TEXCOORD0;
};

struct output_t
{
  float4 Screen : SV_POSITION;
  float2 Texture : TEXCOORD0;
};

output_t main(input_t input)
{
  output_t output;
  output.Screen = float4(input.Position, 1);
  output.Texture = input.Texture;
  return output;
}