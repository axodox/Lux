#pragma once
#include "pch.h"

namespace Lux::Graphics
{
  class d3d11_renderer
  {
  private:
    winrt::com_ptr<ID3D11Device> _device;
    winrt::com_ptr<ID3D11DeviceContext> _context;

  public:
    d3d11_renderer(const winrt::com_ptr<IDXGIAdapter>& dxgiAdapter);

    const winrt::com_ptr<ID3D11Device>& device() const;
    const winrt::com_ptr<ID3D11DeviceContext>& context() const;
  };
}