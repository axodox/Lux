#include "pch.h"
#include "D3D11DesktopCapture.h"
#include "BitwiseOperators.h"

using namespace Lux::BitwiseOperations;
using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation::Metadata;
using namespace winrt::Windows::Graphics;
using namespace winrt::Windows::Graphics::Capture;
using namespace winrt::Windows::Graphics::DirectX;
using namespace winrt::Windows::Graphics::DirectX::Direct3D11;
using namespace ::Windows::Graphics::DirectX::Direct3D11;

namespace Lux::Graphics
{
  winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice get_winrt_device(const winrt::com_ptr<ID3D11Device>& device)
  {
    auto dxgiDevice = device.as<IDXGIDevice>();
    IDirect3DDevice winrtDevice{ nullptr };
    check_hresult(CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(), reinterpret_cast<::IInspectable**>(winrt::put_abi(winrtDevice))));
    return winrtDevice;
  }

  d3d11_desktop_capture::output_info d3d11_desktop_capture::get_output_info()
  {
    output_info info;

    DXGI_OUTPUT_DESC1 desc;
    check_hresult(_output.as<IDXGIOutput6>()->GetDesc1(&desc));

    info.monitor = desc.Monitor;

    info.size.Width = desc.DesktopCoordinates.right - desc.DesktopCoordinates.left;
    info.size.Height = desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top;
    
    switch (desc.ColorSpace)
    {
    case DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020:
      info.format = DirectXPixelFormat::R16G16B16A16Float;
      break;
    default:
      info.format = DirectXPixelFormat::R8G8B8A8UIntNormalized;
      break;
    }

    return info;
  }

  void d3d11_desktop_capture::ensure_resources()
  {
    lock_guard<mutex> lock(_mutex);

    auto outputInfo = get_output_info();
    if (!_resources)
    {
      _resources = make_unique<capture_resources>();     

      auto factory = get_activation_factory<GraphicsCaptureItem>().as<IGraphicsCaptureItemInterop>();
      check_hresult(factory->CreateForMonitor(outputInfo.monitor, guid_of<GraphicsCaptureItem>(), put_abi(_resources->capture_item)));

      auto size = _resources->capture_item.Size();
      auto winrtDevice = get_winrt_device(_device);
      _resources->frame_pool = Direct3D11CaptureFramePool::CreateFreeThreaded(
        winrtDevice,
        outputInfo.format,
        2,
        size);
      _resources->frame_arrived_revoker = _resources->frame_pool.FrameArrived(auto_revoke, { this, &d3d11_desktop_capture::on_frame_arrived });
      _resources->capture_session = _resources->frame_pool.CreateCaptureSession(_resources->capture_item);
      _resources->capture_session.IsCursorCaptureEnabled(false);
      if (ApiInformation::IsPropertyPresent(L"Windows.Graphics.Capture.GraphicsCaptureSession", L"IsBorderRequired"))
      {
        _resources->capture_session.IsBorderRequired(false);
      }
      _resources->capture_session.StartCapture();

      _output_info = outputInfo;
    }
    else if(!are_equal(_output_info, outputInfo))
    {
      _textureA.reset();
      _textureB.reset();

      auto size = _resources->capture_item.Size();
      auto winrtDevice = get_winrt_device(_device);
      _resources->frame_pool.Recreate(
        winrtDevice,
        outputInfo.format,
        2,
        size);

      _output_info = outputInfo;
    }    
  }

  void d3d11_desktop_capture::on_frame_arrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender, const winrt::Windows::Foundation::IInspectable& /*eventArgs*/)
  {
    _frame_arrived_event.set();
  }

  d3d11_desktop_capture::d3d11_desktop_capture(const winrt::com_ptr<ID3D11Device>& device, const winrt::com_ptr<IDXGIOutput5>& output) :
    _device(device),
    _output(output)
  { }

  d3d11_desktop_capture_state d3d11_desktop_capture::try_lock_frame(std::chrono::duration<uint32_t, std::milli> timeout, d3d11_texture_2d*& frame)
  {
    frame = nullptr;
    ensure_resources();

    if (!_resources) return d3d11_desktop_capture_state::unavailable;

    if (_frame_arrived_event.wait(timeout))
    {
      _resources->last_frame = _resources->frame_pool.TryGetNextFrame();

      if (_resources->last_frame == nullptr) return d3d11_desktop_capture_state::unavailable;     

      com_ptr<ID3D11Texture2D> texture;
      auto surface = _resources->last_frame.Surface();
      auto surfaceAccess = surface.as<IDirect3DDxgiInterfaceAccess>();
      check_hresult(surfaceAccess->GetInterface(guid_of<ID3D11Texture2D>(), texture.put_void()));

      if (_textureA != nullptr && _textureA->texture() == texture)
      {
        frame = _textureA.get();
      }
      else if (_textureB != nullptr && _textureB->texture() == texture)
      {
        frame = _textureB.get();
      }
      else
      {
        _textureB = move(_textureA);
        _textureA = make_unique<d3d11_texture_2d>(texture);
        frame = _textureA.get();
      }

      return d3d11_desktop_capture_state::ready;
    }
    else
    {
      return d3d11_desktop_capture_state::timeout;
    }
  }

  void d3d11_desktop_capture::unlock_frame()
  {
    lock_guard<mutex> lock(_mutex);
    if (!_resources || !_resources->last_frame) return;

    _resources->last_frame.Close();
    _resources->last_frame = nullptr;
  }

  bool d3d11_desktop_capture::is_hdr() const
  {
    return _output_info.format == DirectXPixelFormat::R16G16B16A16Float;
  }
}