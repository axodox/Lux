#include "pch.h"
#include "D3D11Resource.h"

using namespace std;
using namespace winrt;

namespace Lux::Graphics
{
  d3d11_resource::d3d11_resource(const com_ptr<ID3D11Resource>& resource) :
    _resource(resource)
  { }

  const com_ptr<ID3D11Resource>& d3d11_resource::resource() const
  {
    return _resource;
  }

  void d3d11_resource::write(const com_ptr<ID3D11DeviceContext>& context, const array_view<uint8_t>& data) const
  {
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    check_hresult(context->Map(_resource.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));

    memcpy(mappedSubresource.pData, data.data(), min(uint32_t(data.size()), mappedSubresource.DepthPitch));
    
    context->Unmap(_resource.get(), 0);
  }
  
  vector<uint8_t> d3d11_resource::read(const com_ptr<ID3D11DeviceContext>& context) const
  {
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    check_hresult(context->Map(_resource.get(), 0, D3D11_MAP_READ, 0, &mappedSubresource));

    vector<uint8_t> data(mappedSubresource.DepthPitch);
    memcpy(data.data(), mappedSubresource.pData, mappedSubresource.DepthPitch);

    context->Unmap(_resource.get(), 0);
    return data;
  }
}