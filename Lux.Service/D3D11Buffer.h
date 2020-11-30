#pragma once
#include "D3D11Resource.h"

namespace Lux::Graphics
{
  class d3d11_buffer : public d3d11_resource
  {
  protected:
    winrt::com_ptr<ID3D11Buffer> _buffer;

  public:
    d3d11_buffer(const winrt::com_ptr<ID3D11Buffer>& buffer);

    const winrt::com_ptr<ID3D11Buffer>& buffer() const;
  };
}