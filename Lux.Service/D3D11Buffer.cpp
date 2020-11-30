#include "pch.h"
#include "D3D11Buffer.h"

using namespace winrt;

namespace Lux::Graphics
{
  d3d11_buffer::d3d11_buffer(const com_ptr<ID3D11Buffer>& buffer) :
    d3d11_resource(buffer),
    _buffer(buffer)
  { }

  const com_ptr<ID3D11Buffer>& d3d11_buffer::buffer() const
  {
    return _buffer;
  }
}