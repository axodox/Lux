#include "pch.h"
#include "D3D11ConstantBuffer.h"

using namespace std;
using namespace winrt;

namespace Lux::Graphics
{
  d3d11_constant_buffer d3d11_constant_buffer::make_dynamic(const com_ptr<ID3D11Device>& device, uint32_t capacity)
  {
    CD3D11_BUFFER_DESC desc(
      (capacity % 16 == 0 ? capacity : capacity + 16 - capacity % 16),
      D3D11_BIND_CONSTANT_BUFFER,
      D3D11_USAGE_DYNAMIC,
      D3D11_CPU_ACCESS_WRITE
    );

    com_ptr<ID3D11Buffer> buffer;
    check_hresult(device->CreateBuffer(&desc, nullptr, buffer.put()));

    return d3d11_constant_buffer(buffer);
  }
  
  d3d11_constant_buffer::d3d11_constant_buffer(const com_ptr<ID3D11Buffer>& buffer) :
    d3d11_buffer(buffer)
  { }

  void d3d11_constant_buffer::set(const com_ptr<ID3D11DeviceContext> & context, d3d11_shader_stage stage, uint32_t slot) const
  {
    auto buffer = _buffer.get();

    switch (stage)
    {
    case d3d11_shader_stage::cs:
      context->CSSetConstantBuffers(slot, 1, &buffer);
      break;
    case d3d11_shader_stage::vs:
      context->VSSetConstantBuffers(slot, 1, &buffer);
      break;
    case d3d11_shader_stage::ps:
      context->PSSetConstantBuffers(slot, 1, &buffer);
      break;
    default:
      throw std::out_of_range("Invalid shader stage for constant buffer!");
    }
  }
}