#pragma once
#include "pch.h"
#include "D3D11ShaderStage.h"

namespace Lux::Graphics
{
  struct d3d11_sampler_state
  {
  private:
    winrt::com_ptr<ID3D11SamplerState> _state;

  public:
    d3d11_sampler_state(const winrt::com_ptr<ID3D11Device>& device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);

    const winrt::com_ptr<ID3D11SamplerState>& state() const;

    void set(const winrt::com_ptr<ID3D11DeviceContext>& context, d3d11_shader_stage stage, int slot = 0) const;
  };
}