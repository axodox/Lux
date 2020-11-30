#include "pch.h"
#include "D3D11Texture2D.h"

using namespace std;
using namespace winrt;

namespace Lux::Graphics
{
  com_ptr<ID3D11ShaderResourceView> d3d11_texture_2d::make_view(const com_ptr<ID3D11Texture2D>& texture)
  {
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);

    if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
    {
      com_ptr<ID3D11Device> device;
      texture->GetDevice(device.put());

      com_ptr<ID3D11ShaderResourceView> view;
      check_hresult(device->CreateShaderResourceView(texture.get(), nullptr, view.put()));
      return view;
    }
    else
    {
      return nullptr;
    }
  }
  
  d3d11_texture_2d d3d11_texture_2d::make_immutable(const com_ptr<ID3D11Device>& device, DXGI_FORMAT format, uint32_t width, uint32_t height, const array_view<uint8_t>& pixels)
  {
    CD3D11_TEXTURE2D_DESC desc(format, width, height);
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.ArraySize = 1;
    desc.MipLevels = 1;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = pixels.data();
    data.SysMemPitch = uint32_t(pixels.size());

    vector<D3D11_SUBRESOURCE_DATA> resources;
    resources.push_back(data);

    com_ptr<ID3D11Texture2D> texture;
    check_hresult(device->CreateTexture2D(&desc, resources.data(), texture.put()));

    return d3d11_texture_2d(texture);
  }

  d3d11_texture_2d::d3d11_texture_2d(const com_ptr<ID3D11Texture2D>& texture) :
    d3d11_resource(texture),
    _texture(texture),
    _view(make_view(texture))
  { }

  const com_ptr<ID3D11Texture2D>& d3d11_texture_2d::texture() const
  {
    return _texture;
  }

  const com_ptr<ID3D11ShaderResourceView>& d3d11_texture_2d::view() const
  {
    return _view;
  }

  void d3d11_texture_2d::set(const com_ptr<ID3D11DeviceContext>& context, d3d11_shader_stage stage, uint32_t slot) const
  {
    auto view = _view.get();

    switch (stage)
    {
    case d3d11_shader_stage::cs:
      context->CSSetShaderResources(slot, 1, &view);
      break;
    case d3d11_shader_stage::vs:
      context->VSSetShaderResources(slot, 1, &view);
      break;
    case d3d11_shader_stage::ps:
      context->PSSetShaderResources(slot, 1, &view);
      break;
    default:
      throw out_of_range("Invalid shader stage for texture!");
    }
  }
}