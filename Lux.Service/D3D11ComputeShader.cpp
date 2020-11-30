#include "pch.h"
#include "D3D11ComputeShader.h"
#include "IO.h"

using namespace Lux::IO;
using namespace winrt;

namespace Lux::Graphics
{
  d3d11_compute_shader::d3d11_compute_shader(const winrt::com_ptr<ID3D11Device>& device, const std::filesystem::path& path)
  {
    auto source = load_file(path);
    check_hresult(device->CreateComputeShader(source.data(), source.size(), nullptr, _shader.put()));
  }

  void d3d11_compute_shader::run(const winrt::com_ptr<ID3D11DeviceContext>& context, uint32_t x, uint32_t y, uint32_t z) const
  {
    context->CSSetShader(_shader.get(), 0, 0);
    context->Dispatch(x, y, z);
  }
}