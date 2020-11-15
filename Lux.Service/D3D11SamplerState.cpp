#include "pch.h"
#include "D3D11SamplerState.h"

using namespace std;
using namespace winrt;

namespace Lux::Graphics
{
  d3d11_sampler_state::d3d11_sampler_state(const com_ptr<ID3D11Device>& device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
  {
    float borderColor[4] = { 0.f, 0.f, 0.f, 0.f };
    CD3D11_SAMPLER_DESC sd(
      filter,
      addressMode,
      addressMode,
      addressMode,
      0.f,
      0,
      D3D11_COMPARISON_NEVER,
      borderColor,
      0.f,
      D3D11_FLOAT32_MAX
    );

    check_hresult(device->CreateSamplerState(&sd, _state.put()));
  }

  const com_ptr<ID3D11SamplerState>& d3d11_sampler_state::state() const
  {
    return _state;
  }

  void d3d11_sampler_state::set(const com_ptr<ID3D11DeviceContext>& context, d3d11_shader_stage stage, int slot) const
  {
    auto state = _state.get();

    switch (stage)
    {
    case d3d11_shader_stage::cs:
      context->CSSetSamplers(slot, 1, &state);
      break;
    case d3d11_shader_stage::vs:
      context->VSSetSamplers(slot, 1, &state);
      break;
    case d3d11_shader_stage::ps:
      context->PSSetSamplers(slot, 1, &state);
      break;
    default:
      throw out_of_range("Invalid shader stage for texture!");
    }
  }
}