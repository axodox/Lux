#pragma once
#include "D3D11Resource.h"
#include "D3D11ShaderStage.h"

namespace Lux::Graphics
{
  class d3d11_texture_2d : public d3d11_resource
  {
  private:
    static winrt::com_ptr<ID3D11ShaderResourceView> make_view(const winrt::com_ptr<ID3D11Texture2D>& texture);
    static d3d11_texture_2d make_immutable(const winrt::com_ptr<ID3D11Device>& device, DXGI_FORMAT format, uint32_t width, uint32_t height, const winrt::array_view<uint8_t>& data);

    winrt::com_ptr<ID3D11Texture2D> _texture;
    winrt::com_ptr<ID3D11ShaderResourceView> _view;

  public:
    d3d11_texture_2d(const winrt::com_ptr<ID3D11Texture2D>& texture);

    const winrt::com_ptr<ID3D11Texture2D>& texture() const;
    const winrt::com_ptr<ID3D11ShaderResourceView>& view() const;

    template<typename TItem>
    static d3d11_texture_2d make_immutable(const winrt::com_ptr<ID3D11Device>& device, DXGI_FORMAT format, uint32_t width, uint32_t height, const winrt::array_view<TItem>& pixels)
    {
      return make_immutable(device, format, width, height, winrt::array_view<uint8_t>(pixels.data(), pixels.data() + pixels.size()));
    }

    void set(const winrt::com_ptr<ID3D11DeviceContext>& context, d3d11_shader_stage stage, uint32_t slot = 0u) const;
  };
}