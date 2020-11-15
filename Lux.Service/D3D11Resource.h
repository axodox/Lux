#pragma once
#include "pch.h"

namespace Lux::Graphics
{
  class d3d11_resource
  {
  protected:
    winrt::com_ptr<ID3D11Resource> _resource;

  public:
    d3d11_resource(const winrt::com_ptr<ID3D11Resource>& resource);

    const winrt::com_ptr<ID3D11Resource>& resource() const;

    void write(const winrt::com_ptr<ID3D11DeviceContext>& context, const winrt::array_view<uint8_t>& data) const;

    std::vector<uint8_t> read(const winrt::com_ptr<ID3D11DeviceContext>& context) const;
  };
}