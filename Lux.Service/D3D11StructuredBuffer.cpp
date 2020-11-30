#include "pch.h"
#include "D3D11StructuredBuffer.h"

using namespace std;
using namespace winrt;

namespace Lux::Graphics
{
  d3d11_structured_buffer::d3d11_structured_buffer(
    const com_ptr<ID3D11Buffer>& buffer, 
    const com_ptr<ID3D11ShaderResourceView>& view, 
    const com_ptr<ID3D11UnorderedAccessView>& uav, 
    uint32_t itemSize, 
    uint32_t capacity) :
    d3d11_buffer(buffer),
    _view(view),
    _uav(uav),
    _item_size(itemSize),
    _capacity(capacity)
  { }

  d3d11_structured_buffer d3d11_structured_buffer::make_immutable(const com_ptr<ID3D11Device>& device, const array_view<uint8_t>& items, uint32_t itemSize)
  {
    CD3D11_BUFFER_DESC desc{
      uint32_t(items.size()),
      D3D11_BIND_SHADER_RESOURCE,
      D3D11_USAGE_IMMUTABLE,
      0,
      D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
      itemSize
    };

    D3D11_SUBRESOURCE_DATA data{ items.data() };
    com_ptr<ID3D11Buffer> buffer;
    check_hresult(device->CreateBuffer(&desc, &data, buffer.put()));

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc{};
    viewDesc.Format = DXGI_FORMAT_UNKNOWN;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    viewDesc.Buffer.NumElements = uint32_t(items.size()) / itemSize;

    com_ptr<ID3D11ShaderResourceView> view;
    check_hresult(device->CreateShaderResourceView(buffer.get(), &viewDesc, view.put()));

    return d3d11_structured_buffer(buffer, view, nullptr, itemSize, viewDesc.Buffer.NumElements);
  }

  d3d11_structured_buffer d3d11_structured_buffer::make_writeable(const com_ptr<ID3D11Device>& device, uint32_t itemSize, uint32_t capacity)
  {
    CD3D11_BUFFER_DESC desc(
      itemSize * capacity,
      D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS,
      D3D11_USAGE_DEFAULT,
      0,
      D3D11_RESOURCE_MISC_BUFFER_STRUCTURED,
      itemSize
    );

    com_ptr<ID3D11Buffer> buffer;
    check_hresult(device->CreateBuffer(&desc, nullptr, buffer.put()));

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc{};
    viewDesc.Format = DXGI_FORMAT_UNKNOWN;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    viewDesc.Buffer.NumElements = capacity;

    com_ptr<ID3D11ShaderResourceView> view;
    check_hresult(device->CreateShaderResourceView(buffer.get(), &viewDesc, view.put()));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
    uavDesc.Format = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.NumElements = capacity;

    com_ptr<ID3D11UnorderedAccessView> uav;
    check_hresult(device->CreateUnorderedAccessView(buffer.get(), &uavDesc, uav.put()));

    return d3d11_structured_buffer(buffer, view, uav, itemSize, capacity);
  }

  d3d11_structured_buffer d3d11_structured_buffer::make_staging(const com_ptr<ID3D11Device>& device, uint32_t itemSize, uint32_t capacity)
  {
    CD3D11_BUFFER_DESC desc(
      itemSize * capacity,
      0,
      D3D11_USAGE_STAGING,
      D3D11_CPU_ACCESS_READ
    );

    com_ptr<ID3D11Buffer> buffer;
    check_hresult(device->CreateBuffer(&desc, nullptr, buffer.put()));

    return d3d11_structured_buffer(buffer, nullptr, nullptr, itemSize, capacity);
  }

  void d3d11_structured_buffer::copy_to(const com_ptr<ID3D11DeviceContext>& context, const d3d11_structured_buffer& target)
  {
    context->CopyResource(target._resource.get(), _resource.get());
  }
  
  void d3d11_structured_buffer::set_readonly(const com_ptr<ID3D11DeviceContext>& context, uint32_t slot) const
  {
    auto view = _view.get();
    context->CSSetShaderResources(slot, 1, &view);
  }

  void d3d11_structured_buffer::set_writeable(const com_ptr<ID3D11DeviceContext>& context, uint32_t slot) const
  {
    auto uavInitialCount = 0u;
    auto uav = _uav.get();
    context->CSSetUnorderedAccessViews(slot, 1, &uav, &uavInitialCount);
  }
}