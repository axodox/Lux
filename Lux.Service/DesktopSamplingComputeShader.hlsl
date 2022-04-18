SamplerState _sampler : register(s0);
Texture2D _texture : register(t0);
StructuredBuffer<float4> _sampleRects : register(t1);
RWStructuredBuffer<uint4> _sumTexture : register(u0);

groupshared float4 _sampleRect;
groupshared float2 _sampleSize;
groupshared uint4 _sum = uint4(0, 0, 0, 0);

static const uint _randomPrecision = 256;
float urandom(inout uint seed)
{
  seed = seed * 22695477 + 1;
  return float(seed % _randomPrecision) / float(_randomPrecision);
}

cbuffer constants : register(b0)
{
  uint time;
}

#define SAMPLE_POINTS 32

[numthreads(SAMPLE_POINTS, SAMPLE_POINTS, 1)]
void main(
  uint3 index : SV_DispatchThreadID,
  uint3 groupId : SV_GroupID,
  uint3 threadId : SV_GroupThreadID)
{
  bool isLeader = !any(threadId);
  if (isLeader)
  {
    _sampleRect = _sampleRects[groupId.x];
    _sampleSize = (_sampleRect.zy - _sampleRect.xw);
  }
  GroupMemoryBarrierWithGroupSync();

  uint seed = index.x ^ index.y;
  float2 samplePoint = float2(0, 1) + float2(1, -1) * (_sampleRect.xw + _sampleSize.xy * float2(urandom(seed), urandom(seed)));

  float4 color = _texture.SampleLevel(_sampler, samplePoint, 0);
  color.rgb = max(min(color.rgb, 1), 0);

  uint4 value = uint4(255 * color.r, 255 * color.g, 255 * color.b, 1);
  InterlockedAdd(_sum.x, value.x);
  InterlockedAdd(_sum.y, value.y);
  InterlockedAdd(_sum.z, value.z);
  InterlockedAdd(_sum.w, value.w);

  GroupMemoryBarrierWithGroupSync();
  if (isLeader)
  {
    uint4 value = _sum.w > 0 ? _sum.xyzw / _sum.w : 0;
    _sumTexture[groupId.x] = value;
  }
}