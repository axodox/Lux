#pragma once
#include "D3D11Buffer.h"

namespace Lux::Graphics
{
  class d3d11_structured_buffer : public d3d11_buffer
  {
  private:
    winrt::com_ptr<ID3D11ShaderResourceView> _view;
    winrt::com_ptr<ID3D11UnorderedAccessView> _uav;
    uint32_t _item_size;
    uint32_t _capacity;

    d3d11_structured_buffer(
      const winrt::com_ptr<ID3D11Buffer>& buffer,
      const winrt::com_ptr<ID3D11ShaderResourceView>& view,
      const winrt::com_ptr<ID3D11UnorderedAccessView>& uav,
      uint32_t itemSize,
      uint32_t capacity);

    static d3d11_structured_buffer make_immutable(const winrt::com_ptr<ID3D11Device>& device, const winrt::array_view<uint8_t>& data, uint32_t itemSize);
    static d3d11_structured_buffer make_writeable(const winrt::com_ptr<ID3D11Device>& device, uint32_t itemSize, uint32_t capacity);
    static d3d11_structured_buffer make_staging(const winrt::com_ptr<ID3D11Device>& device, uint32_t itemSize, uint32_t capacity);

  public:
    template<typename T>
    static d3d11_structured_buffer make_immutable(const winrt::com_ptr<ID3D11Device>& device, const std::vector<T>& items)
    {
      return make_immutable(device, winrt::array_view<uint8_t>((uint8_t*)items.data(), (uint8_t*)(items.data() + items.size())), sizeof(T));
    }

    template<typename T>
    static d3d11_structured_buffer make_writeable(const winrt::com_ptr<ID3D11Device>& device, uint32_t capacity)
    {
      return make_writeable(device, sizeof(T), capacity);
    }

    template<typename T>
    static d3d11_structured_buffer make_staging(const winrt::com_ptr<ID3D11Device>& device, uint32_t capacity)
    {
      return make_staging(device, sizeof(T), capacity);
    }

    void copy_to(const winrt::com_ptr<ID3D11DeviceContext>& context, const d3d11_structured_buffer& target);

    void set_readonly(const winrt::com_ptr<ID3D11DeviceContext>& context, uint32_t slot = 0) const;
    void set_writeable(const winrt::com_ptr<ID3D11DeviceContext>& context, uint32_t slot = 0) const;
  };
}