#pragma once
#include "pch.h"

namespace Lux::Graphics
{
  class d3d11_compute_shader
  {
  private:
    winrt::com_ptr<ID3D11ComputeShader> _shader;

  public:
    d3d11_compute_shader(const winrt::com_ptr<ID3D11Device>& device, const std::filesystem::path& path);

    void run(const winrt::com_ptr<ID3D11DeviceContext>& context, uint32_t x, uint32_t y = 1, uint32_t z = 1) const;
  };
}