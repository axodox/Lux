#include "pch.h"
#include "DisplayOutputInfo.h"
#include "DisplayOutputInfo.g.cpp"

namespace winrt::Lux::Common::implementation
{
  Windows::Foundation::Collections::IVector<Lux::Common::DisplayOutputInfo> DisplayOutputInfo::GetOutputs()
  {
    com_ptr<IDXGIFactory> dxgiFactory = nullptr;
    check_hresult(CreateDXGIFactory2(0, __uuidof(IDXGIFactory), dxgiFactory.put_void()));

    com_ptr<IDXGIAdapter> dxgiAdapter;
    for (uint32_t adapterIndex = 0u; dxgiFactory->EnumAdapters(adapterIndex, dxgiAdapter.put()) != DXGI_ERROR_NOT_FOUND; adapterIndex++)
    {
      DXGI_ADAPTER_DESC dxgiAdapterDesc;
      check_hresult(dxgiAdapter->GetDesc(&dxgiAdapterDesc));

      auto adapterId = reinterpret_cast<uint64_t&>(dxgiAdapterDesc.AdapterLuid);

      com_ptr<IDXGIOutput> dxgiOutput;
      for (uint32_t outputIndex = 0u; dxgiAdapter->EnumOutputs(outputIndex, dxgiOutput.put()) != DXGI_ERROR_NOT_FOUND; outputIndex++)
      {
        if (!dxgiOutput.try_as<IDXGIOutput2>()) continue;

        DXGI_OUTPUT_DESC desc;
        check_hresult(dxgiOutput->GetDesc(&desc));

        //DISPLAY_DEVICE dev = {};
        //dev.cb = sizeof(dev);
        //EnumDisplayDevices(desc.DeviceName, 0, &dev, 0);

        dxgiOutput = nullptr;
      }
      dxgiAdapter = nullptr;
    }

    return nullptr;
    throw hresult_not_implemented();
  }

  uint64_t DisplayOutputInfo::AdapterId()
  {
    return _adapterId;
  }

  void DisplayOutputInfo::AdapterId(uint64_t value)
  {
    _adapterId = value;
  }

  hstring DisplayOutputInfo::OutputId()
  {
    return _outputId;
  }

  void DisplayOutputInfo::OutputId(hstring const& value)
  {
    _outputId = value;
  }

  hstring DisplayOutputInfo::Name()
  {
    return _name;
  }

  void DisplayOutputInfo::Name(hstring const& value)
  {
    _name = value;
  }
}
