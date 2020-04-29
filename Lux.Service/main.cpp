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
protected:
  d3d11_renderer(const tuple<com_ptr<ID3D11Device>, com_ptr<ID3D11DeviceContext>>& args) :
    device(get<0>(args)),
    context(get<1>(args))
  { }

private:
  static tuple<com_ptr<ID3D11Device>, com_ptr<ID3D11DeviceContext>> args(const com_ptr<IDXGIAdapter>& dxgiAdapter)
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

    tuple<com_ptr<ID3D11Device>, com_ptr<ID3D11DeviceContext>> args;
    check_hresult(D3D11CreateDevice(
      dxgiAdapter.get(),
      D3D_DRIVER_TYPE_UNKNOWN,
      nullptr,
      creationFlags,
      featureLevels,
      ARRAYSIZE(featureLevels),
      D3D11_SDK_VERSION,
      get<0>(args).put(),
      nullptr,
      get<1>(args).put()
    ));

    return args;
  }

public:
  const com_ptr<ID3D11Device> device;
  const com_ptr<ID3D11DeviceContext> context;

  d3d11_renderer(const com_ptr<IDXGIAdapter>& dxgiAdapter) : d3d11_renderer(args(dxgiAdapter))
  { }
};

struct d3d11_texture_2d
{
  const com_ptr<ID3D11Texture2D> texture;

  d3d11_texture_2d(const com_ptr<ID3D11Texture2D> texture) :
    texture(texture)
  { }
};

struct d3d11_render_target_2d : public d3d11_texture_2d
{
private:
  static com_ptr<ID3D11RenderTargetView> get_view(const com_ptr<ID3D11Texture2D>& texture)
  {
    com_ptr<ID3D11Device> device;
    texture->GetDevice(device.put());

    com_ptr<ID3D11RenderTargetView> view;
    check_hresult(device->CreateRenderTargetView(texture.get(), nullptr, view.put()));

    return view;
  }

  static D3D11_VIEWPORT get_view_port(const com_ptr<ID3D11Texture2D>& texture)
  {
    D3D11_TEXTURE2D_DESC desc;
    texture->GetDesc(&desc);
    return { 0, 0, (float)desc.Width, (float)desc.Height, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH };
  }

public:
  const com_ptr<ID3D11RenderTargetView> view;
  const D3D11_VIEWPORT view_port;

  d3d11_render_target_2d(const com_ptr<ID3D11Texture2D>& texture) :
    d3d11_texture_2d(texture),
    view(get_view(texture)),
    view_port(get_view_port(texture))
  { }

  void set(const com_ptr<ID3D11DeviceContext>& context) const
  {
    array<ID3D11RenderTargetView*, 1> targets = { view.get() };
    context->OMSetRenderTargets(1, targets.data(), nullptr);
    context->RSSetViewports(1, &view_port);
  }

  void clear(const com_ptr<ID3D11DeviceContext>& context, const array<float, 4>& color = {}) const
  {
    context->ClearRenderTargetView(view.get(), color.data());
  }
};

struct d3d11_renderer_with_swap_chain : public d3d11_renderer
{
private:
  unordered_map<void*, unique_ptr<d3d11_render_target_2d>> _buffers;

  typedef tuple<com_ptr<IDXGISwapChain>, com_ptr<ID3D11Device>, com_ptr<ID3D11DeviceContext>> args_t;

  d3d11_renderer_with_swap_chain(const args_t& args) :
    d3d11_renderer({ get<1>(args), get<2>(args) }),
    swap_chain(get<0>(args))
  { }

  static args_t args(const com_ptr<IDXGIAdapter>& dxgiAdapter, const handle& windowHandle)
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

    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = (HWND)windowHandle.get();
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    args_t args;
    check_hresult(D3D11CreateDeviceAndSwapChain(
      dxgiAdapter.get(),
      D3D_DRIVER_TYPE_UNKNOWN,
      nullptr,
      creationFlags,
      featureLevels,
      ARRAYSIZE(featureLevels),
      D3D11_SDK_VERSION,
      &swapChainDesc,
      get<0>(args).put(),
      get<1>(args).put(),
      nullptr,
      get<2>(args).put()
    ));
    return args;
  }

public:
  const com_ptr<IDXGISwapChain> swap_chain;

  d3d11_renderer_with_swap_chain(const com_ptr<IDXGIAdapter>& dxgiAdapter, const handle& windowHandle) : d3d11_renderer_with_swap_chain(args(dxgiAdapter, windowHandle))
  { }

  const d3d11_render_target_2d& render_target()
  {
    com_ptr<ID3D11Texture2D> texture;
    check_hresult(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), texture.put_void()));

    auto& render_target = _buffers[texture.get()];
    if (!render_target)
    {
      render_target = make_unique<d3d11_render_target_2d>(texture);
    }

    return *render_target.get();
  }
};

LRESULT CALLBACK debug_message_handler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProcW(windowHandle, message, wParam, lParam);
    break;
  }

  return 0;
}

handle create_debug_window()
{
  handle windowCreatedEvent = handle(CreateEvent(0, false, false, nullptr));

  auto hEvent = windowCreatedEvent.get();
  handle hWnd;
  thread([&hWnd, hEvent] {
    auto hInstance = GetModuleHandle(NULL);

    WNDCLASSEXW classDescription = { 0 };
    classDescription.cbSize = sizeof(WNDCLASSEX);
    classDescription.hInstance = hInstance;
    classDescription.lpfnWndProc = debug_message_handler;
    classDescription.lpszClassName = L"DebugWindow";
    classDescription.style = CS_HREDRAW | CS_VREDRAW;
    classDescription.hCursor = LoadCursor(NULL, IDC_ARROW);
    classDescription.hbrBackground = (HBRUSH)COLOR_WINDOW;

    RegisterClassExW(&classDescription);

    hWnd = handle(CreateWindowExW(NULL, L"DebugWindow", L"Debug Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, 300, 300, NULL, NULL, hInstance, nullptr));

    ShowWindow((HWND)hWnd.get(), SW_SHOW);
    SetEvent(hEvent);

    MSG wMessage;

    while (GetMessage(&wMessage, NULL, 0, 0))
    {
      TranslateMessage(&wMessage);
      DispatchMessage(&wMessage);
    }
    }).detach();

    WaitForSingleObject(hEvent, INFINITE);
    return hWnd;
}



int main()
{
  init_apartment();

  auto dxgiOutput = get_default_output();

  com_ptr<IDXGIAdapter> dxgiAdapter;
  check_hresult(dxgiOutput->GetParent(__uuidof(IDXGIAdapter), dxgiAdapter.put_void()));

  auto window = create_debug_window();

  d3d11_renderer_with_swap_chain d3d11Renderer(dxgiAdapter, window);

  com_ptr<IDXGIOutputDuplication> dxgiOutputDuplication;
  check_hresult(dxgiOutput->DuplicateOutput(d3d11Renderer.device.get(), dxgiOutputDuplication.put()));


  while (true)
  {
    DXGI_OUTDUPL_FRAME_INFO dxgiFrameInfo;
    com_ptr<IDXGIResource> dxgiResource;
    check_hresult(dxgiOutputDuplication->AcquireNextFrame(1000u, &dxgiFrameInfo, dxgiResource.put()));

    auto& target = d3d11Renderer.render_target();
    target.set(d3d11Renderer.context);
    target.clear(d3d11Renderer.context, { 1.f, 0.f, 0.f, 1.f });
    


    d3d11Renderer.swap_chain->Present(1, 0);

    check_hresult(dxgiOutputDuplication->ReleaseFrame());

    
  }
}
