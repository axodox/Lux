#include "pch.h"
#include "DXGIOutput.h"

using namespace winrt;

namespace Lux::Graphics
{
  winrt::com_ptr<IDXGIOutput5> get_default_output()
  {
    com_ptr<IDXGIFactory1> dxgiFactory = nullptr;
    check_hresult(CreateDXGIFactory2(0, __uuidof(IDXGIFactory1), dxgiFactory.put_void()));

    com_ptr<IDXGIAdapter1> dxgiAdapter;
    for (uint32_t adapterIndex = 0u; dxgiFactory->EnumAdapters1(adapterIndex, dxgiAdapter.put()) != DXGI_ERROR_NOT_FOUND; adapterIndex++)
    {
      com_ptr<IDXGIOutput> dxgiOutput;
      for (uint32_t outputIndex = 0u; dxgiAdapter->EnumOutputs(outputIndex, dxgiOutput.put()) != DXGI_ERROR_NOT_FOUND; outputIndex++)
      {
        auto dxgiOutput2 = dxgiOutput.try_as<IDXGIOutput5>();
        if (dxgiOutput2) return dxgiOutput2;

        dxgiOutput = nullptr;
      }
      dxgiAdapter = nullptr;
    }

    return nullptr;
  }

  com_ptr<IDXGIAdapter> get_adapter(const com_ptr<IDXGIOutput5>& output)
  {
    com_ptr<IDXGIAdapter> adapter;
    check_hresult(output->GetParent(__uuidof(IDXGIAdapter), adapter.put_void()));
    return adapter;
  }
}