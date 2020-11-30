#pragma once
#include "D3D11Texture2D.h"

namespace Lux::Graphics
{
  enum class d3d11_desktop_duplication_state
  {
    unavailable,
    timeout,
    ready
  };

  class d3d11_desktop_duplication
  {
  private:
    std::function<winrt::com_ptr<IDXGIOutput5>()> _outputProvider;
    winrt::com_ptr<IDXGIOutputDuplication> _outputDuplication;
    std::unique_ptr<d3d11_texture_2d> _texture;
    winrt::com_ptr<ID3D11Device> _device;
    winrt::com_ptr<IDXGIOutput5> _output;

    bool _isHdr = false;

  public:
    d3d11_desktop_duplication(const winrt::com_ptr<ID3D11Device>& device, const winrt::com_ptr<IDXGIOutput5>& output);

    d3d11_desktop_duplication_state try_lock_frame(std::chrono::duration<uint32_t, std::milli> timeout, d3d11_texture_2d*& texture);
    void unlock_frame();

    bool is_hdr() const;
  };
}