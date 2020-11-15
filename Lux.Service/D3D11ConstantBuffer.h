#pragma once
#include "D3D11Buffer.h"
#include "D3D11ShaderStage.h"

namespace Lux::Graphics
{
  class d3d11_constant_buffer : public d3d11_buffer
  {
  private:
    static d3d11_constant_buffer make_dynamic(const winrt::com_ptr<ID3D11Device>& device, uint32_t capacity);

  public:
    d3d11_constant_buffer(const winrt::com_ptr<ID3D11Buffer>& buffer);

    template<typename T>
    static d3d11_constant_buffer make_dynamic(const winrt::com_ptr<ID3D11Device>& device)
    {
      return make_dynamic(device, sizeof(T));
    }

    template<typename T>
    void write(const winrt::com_ptr<ID3D11DeviceContext>& context, const T& value)
    {
      d3d11_buffer::write(context, winrt::array_view{ (uint8_t*)&value, sizeof(value) });
    }

    void set(const winrt::com_ptr<ID3D11DeviceContext>& context, d3d11_shader_stage stage, uint32_t slot = 0) const;
  };
}