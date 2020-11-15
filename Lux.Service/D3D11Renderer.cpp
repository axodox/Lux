#include "pch.h"
#include "D3D11Renderer.h"

using namespace winrt;

namespace Lux::Graphics
{
  d3d11_renderer::d3d11_renderer(const com_ptr<IDXGIAdapter>& dxgiAdapter)
  {
    D3D_FEATURE_LEVEL featureLevels[] =
    {
      D3D_FEATURE_LEVEL_12_1,
      D3D_FEATURE_LEVEL_12_0,
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
    };

    uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    check_hresult(D3D11CreateDevice(
      dxgiAdapter.get(),
      D3D_DRIVER_TYPE_UNKNOWN,
      nullptr,
      creationFlags,
      featureLevels,
      ARRAYSIZE(featureLevels),
      D3D11_SDK_VERSION,
      _device.put(),
      nullptr,
      _context.put()
    ));
  }

  const winrt::com_ptr<ID3D11Device>& d3d11_renderer::device() const
  {
    return _device;
  }

  const winrt::com_ptr<ID3D11DeviceContext>& d3d11_renderer::context() const
  {
    return _context;
  }
}