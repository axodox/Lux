#include "pch.h"

using namespace std;
using namespace winrt;

com_ptr<IDXGIOutput2> get_default_output()
{
  com_ptr<IDXGIFactory> dxgiFactory = nullptr;
  check_hresult(CreateDXGIFactory2(0, __uuidof(IDXGIFactory), dxgiFactory.put_void()));

  com_ptr<IDXGIAdapter> dxgiAdapter;
  for (uint32_t adapterIndex = 0u; dxgiFactory->EnumAdapters(adapterIndex, dxgiAdapter.put()) != DXGI_ERROR_NOT_FOUND; adapterIndex++)
  {
    DXGI_ADAPTER_DESC dxgiAdapterDesc;
    check_hresult(dxgiAdapter->GetDesc(&dxgiAdapterDesc));

    com_ptr<IDXGIOutput> dxgiOutput;
    for (uint32_t outputIndex = 0u; dxgiAdapter->EnumOutputs(outputIndex, dxgiOutput.put()) != DXGI_ERROR_NOT_FOUND; outputIndex++)
    {
      auto dxgiOutput2 = dxgiOutput.try_as<IDXGIOutput2>();
      if (dxgiOutput2) return dxgiOutput2;

      dxgiOutput = nullptr;
    }
    dxgiAdapter = nullptr;
  }

  return nullptr;
}

struct d3d11_renderer
{
  com_ptr<ID3D11Device> device;
  com_ptr<ID3D11DeviceContext> context;
};

d3d11_renderer create_renderer(const com_ptr<IDXGIAdapter>& dxgiAdapter)
{
  D3D_FEATURE_LEVEL featureLevels[] =
  {
    D3D_FEATURE_LEVEL_12_1,
    D3D_FEATURE_LEVEL_12_0,
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0
  };

  uint32_t creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
  creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  d3d11_renderer result;
  check_hresult(D3D11CreateDevice(
    dxgiAdapter.get(), 
    D3D_DRIVER_TYPE_UNKNOWN,
    nullptr,
    creationFlags,
    featureLevels,
    ARRAYSIZE(featureLevels),
    D3D11_SDK_VERSION,
    result.device.put(),
    nullptr,
    result.context.put()
  ));
  return result;
}

int main()
{
  init_apartment();

  auto dxgiOutput = get_default_output();

  com_ptr<IDXGIAdapter> dxgiAdapter;
  check_hresult(dxgiOutput->GetParent(__uuidof(IDXGIAdapter), dxgiAdapter.put_void()));

  auto d3d11Renderer = create_renderer(dxgiAdapter);

  com_ptr<IDXGIOutputDuplication> dxgiOutputDuplication;
  check_hresult(dxgiOutput->DuplicateOutput(d3d11Renderer.device.get(), dxgiOutputDuplication.put()));

  while (true)
  {
    DXGI_OUTDUPL_FRAME_INFO dxgiFrameInfo;
    com_ptr<IDXGIResource> dxgiResource;
    check_hresult(dxgiOutputDuplication->AcquireNextFrame(1000u, &dxgiFrameInfo, dxgiResource.put()));

    check_hresult(dxgiOutputDuplication->ReleaseFrame());    
  }
}
