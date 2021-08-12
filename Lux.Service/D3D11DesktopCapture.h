#pragma once
#include "D3D11Texture2D.h"
#include "ResetEvent.h"

namespace Lux::Graphics
{
  enum class d3d11_desktop_capture_state
  {
    unavailable,
    timeout,
    ready
  };

  class d3d11_desktop_capture
  {
  private:
    winrt::com_ptr<ID3D11Device> _device;
    winrt::com_ptr<IDXGIOutput5> _output;

    struct capture_resources
    {
      winrt::Windows::Graphics::Capture::GraphicsCaptureItem capture_item{ nullptr };
      winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool frame_pool{ nullptr };
      winrt::Windows::Graphics::Capture::GraphicsCaptureSession capture_session{ nullptr };
      winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker frame_arrived_revoker;
      winrt::Windows::Graphics::Capture::Direct3D11CaptureFrame last_frame{ nullptr };
    };

    std::unique_ptr<capture_resources> _resources;
    Threading::auto_reset_event _frame_arrived_event;
    std::unique_ptr<d3d11_texture_2d> _textureA, _textureB;
    std::mutex _mutex;

    struct output_info
    {
      HMONITOR monitor;
      winrt::Windows::Graphics::DirectX::DirectXPixelFormat format;
      winrt::Windows::Graphics::SizeInt32 size;
    };
    output_info _output_info = { };

    output_info get_output_info();

    void ensure_resources();
    void on_frame_arrived(const winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool& sender, const winrt::Windows::Foundation::IInspectable& eventArgs);

  public:
    d3d11_desktop_capture(const winrt::com_ptr<ID3D11Device>& device, const winrt::com_ptr<IDXGIOutput5>& output);

    d3d11_desktop_capture_state try_lock_frame(std::chrono::duration<uint32_t, std::milli> timeout, d3d11_texture_2d*& texture);
    void unlock_frame();

    bool is_hdr() const;
  };
}