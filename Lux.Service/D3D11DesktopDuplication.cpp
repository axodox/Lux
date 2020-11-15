#include "pch.h"
#include "D3D11DesktopDuplication.h"

using namespace std;
using namespace std::chrono;
using namespace winrt;

namespace Lux::Graphics
{
  d3d11_desktop_duplication::d3d11_desktop_duplication(const com_ptr<ID3D11Device>& device, const com_ptr<IDXGIOutput5>& output) :
    _device(device),
    _output(output)
  { }

  d3d11_texture_2d* d3d11_desktop_duplication::lock_frame(duration<uint32_t, milli> timeout)
  {
    if (_outputDuplication == nullptr)
    {
      DXGI_OUTPUT_DESC1 desc;
      _output.as<IDXGIOutput6>()->GetDesc1(&desc);

      DXGI_FORMAT format;
      switch (desc.ColorSpace)
      {
      case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
        _isHdr = true;
        format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        break;
      default:
        _isHdr = false;
        format = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
      }

      _output->DuplicateOutput1(_device.get(), 0, 1, &format, _outputDuplication.put());
    }

    if (_outputDuplication != nullptr)
    {
      DXGI_OUTDUPL_FRAME_INFO frameInfo;
      com_ptr<IDXGIResource> resource;
      auto result = _outputDuplication->AcquireNextFrame(timeout.count(), &frameInfo, resource.put());

      switch (result)
      {
      case ERROR_SUCCESS:
      {
        auto texture = resource.as<ID3D11Texture2D>();
        if (!_texture || _texture->texture() != texture)
        {
          _texture = make_unique<d3d11_texture_2d>(texture);
        }

        return _texture.get();
      }
      case DXGI_ERROR_WAIT_TIMEOUT:
        return nullptr;
      default:
        _outputDuplication = nullptr;
        return nullptr;
      }
    }
    else
    {
      return nullptr;
    }
  }

  void d3d11_desktop_duplication::unlock_frame()
  {
    auto result = _outputDuplication->ReleaseFrame();
    if (result == DXGI_ERROR_ACCESS_LOST)
    {
      _outputDuplication = nullptr;
    }
  }

  bool d3d11_desktop_duplication::is_hdr() const
  {
    return _isHdr;
  }
}
