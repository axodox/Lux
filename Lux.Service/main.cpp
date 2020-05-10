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

struct d3d11_resource
{
  const com_ptr<ID3D11Resource> resource;

  d3d11_resource(const com_ptr<ID3D11Resource>& resource) :
    resource(resource)
  { }

  void update(const com_ptr<ID3D11DeviceContext>& context, const array_view<uint8_t>& data) const
  {
    com_ptr<ID3D11Device> device;
    resource->GetDevice(device.put());

    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    check_hresult(context->Map(resource.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource));
    memcpy(mappedSubresource.pData, data.data(), min(data.size(), mappedSubresource.DepthPitch));
    context->Unmap(resource.get(), 0);
  }
};

enum class d3d11_shader_stage
{
  cs,
  vs,
  ps
};

struct d3d11_texture_2d : d3d11_resource
{
private:
  static com_ptr<ID3D11ShaderResourceView> make_view(const com_ptr<ID3D11Texture2D>& texture)
  {
    com_ptr<ID3D11Device> device;
    texture->GetDevice(device.put());

    com_ptr<ID3D11ShaderResourceView> view;
    device->CreateShaderResourceView(texture.get(), nullptr, view.put());
    return view;
  }

public:
  const com_ptr<ID3D11Texture2D> texture;
  const com_ptr<ID3D11ShaderResourceView> view;

  d3d11_texture_2d(const com_ptr<ID3D11Texture2D>& texture) :
    d3d11_resource(texture),
    texture(texture),
    view(make_view(texture))
  { }

  void set(const com_ptr<ID3D11DeviceContext>& context, d3d11_shader_stage stage, uint32_t slot = 0u) const
  {
    const array<ID3D11ShaderResourceView*, 1> views = { view.get() };
    switch (stage)
    {
    case d3d11_shader_stage::cs:
      context->CSSetShaderResources(slot, 1, views.data());
      break;
    case d3d11_shader_stage::vs:
      context->VSSetShaderResources(slot, 1, views.data());
      break;
    case d3d11_shader_stage::ps:
      context->PSSetShaderResources(slot, 1, views.data());
      break;
    default:
      throw out_of_range("Invalid shader stage for texture!");
    }
  }
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

struct d3d11_buffer : public d3d11_resource
{
  const com_ptr<ID3D11Buffer> buffer;

  d3d11_buffer(const com_ptr<ID3D11Buffer>& buffer) :
    d3d11_resource(buffer),
    buffer(buffer)
  { }
};

struct d3d11_vertex_position
{
  array<float, 3> position;

  static inline array<D3D11_INPUT_ELEMENT_DESC, 1> input_desc = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };
};

struct d3d11_vertex_position_texture : public d3d11_vertex_position
{
  array<float, 2> texture;

  static inline array<D3D11_INPUT_ELEMENT_DESC, 2> input_desc = {
    d3d11_vertex_position::input_desc[0],
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };
};

template<typename TVertex>
struct d3d11_vertex_buffer : public d3d11_buffer
{
private:
  d3d11_vertex_buffer(const com_ptr<ID3D11Buffer>& buffer, uint32_t capacity) :
    d3d11_buffer(buffer),
    capacity(capacity)
  { }

  uint32_t _size = 0;

public:
  typedef TVertex vertex_t;
  const uint32_t capacity;

  const array_view<D3D11_INPUT_ELEMENT_DESC> input_desc() const
  {
    return vertex_t::input_desc;
  }

  uint32_t size() const
  {
    return _size;
  }

  static d3d11_vertex_buffer make_immutable(const com_ptr<ID3D11Device>& device, const array_view<vertex_t>& vertices)
  {
    CD3D11_BUFFER_DESC desc(
      sizeof(vertex_t) * vertices.size(),
      D3D11_BIND_VERTEX_BUFFER,
      D3D11_USAGE_IMMUTABLE
    );

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = vertices.data();

    com_ptr<ID3D11Buffer> buffer;
    check_hresult(device->CreateBuffer(&desc, &data, buffer.put()));

    d3d11_vertex_buffer result(buffer, vertices.size());
    result._size = vertices.size();
    return result;
  }

  static d3d11_vertex_buffer make_dynamic(const com_ptr<ID3D11Device>& device, uint32_t capacity)
  {
    CD3D11_BUFFER_DESC desc(
      sizeof(vertex_t) * capacity,
      D3D11_BIND_VERTEX_BUFFER,
      D3D11_USAGE_DYNAMIC,
      D3D11_CPU_ACCESS_WRITE
    );

    com_ptr<ID3D11Buffer> buffer;
    check_hresult(device->CreateBuffer(&desc, nullptr, buffer.put()));

    return d3d11_vertex_buffer(buffer, capacity);
  }

  void update(const com_ptr<ID3D11DeviceContext>& context, const array_view<vertex_t>& vertices) const
  {
    _size = min(vertices.size() * sizeof(vertex_t), capacity);
    d3d11_buffer::update(context, { vertices.data(), _size });
  }

  void set(const com_ptr<ID3D11DeviceContext>& context, uint32_t slot = 0) const
  {
    uint32_t stride = sizeof(vertex_t);
    uint32_t offset = 0;
    array<ID3D11Buffer*, 1> buffers = { buffer.get() };
    context->IASetVertexBuffers(slot, 1u, buffers.data(), &stride, &offset);
  }
};

template<typename TConstant>
struct d3d11_constant_buffer : public d3d11_buffer
{
private:
  d3d11_constant_buffer(const com_ptr<ID3D11Buffer>& buffer, uint32_t capacity) :
    d3d11_buffer(buffer),
    capacity(capacity)
  { }

public:
  typedef TConstant constant_t;
  const uint32_t capacity;

  static d3d11_constant_buffer make_dynamic(const com_ptr<ID3D11Device>& device)
  {
    auto capacity = sizeof(constant_t);
    CD3D11_BUFFER_DESC desc(
      (capacity % 16 == 0 ? capacity : capacity + 16 - capacity % 16),
      D3D11_BIND_CONSTANT_BUFFER,
      D3D11_USAGE_DYNAMIC,
      D3D11_CPU_ACCESS_WRITE
    );

    com_ptr<ID3D11Buffer> buffer;
    check_hresult(device->CreateBuffer(&desc, nullptr, buffer.put()));

    return d3d11_constant_buffer(buffer, capacity);
  }

  void update(const constant_t& value) const
  {
    d3d11_buffer::update({ &value, capacity });
  }

  void set(const com_ptr<ID3D11DeviceContext>& context, d3d11_shader_stage stage, uint32_t slot = 0) const
  {
    array<ID3D11Buffer*, 1> buffers = { buffer.get() };
    switch (stage)
    {
    case d3d11_shader_stage::cs:
      context->CSSetConstantBuffers(slot, 1, buffers);
      break;
    case d3d11_shader_stage::vs:
      context->VSSetConstantBuffers(slot, 1, buffers);
      break;
    case d3d11_shader_stage::ps:
      context->PSSetConstantBuffers(slot, 1, buffers);
      break;
    default:
      throw out_of_range("Invalid shader stage for constant buffer!");
    }
  }
};

template<typename TVertex>
struct d3d11_simple_mesh
{
  const d3d11_vertex_buffer<TVertex> vertex_buffer;
  const D3D11_PRIMITIVE_TOPOLOGY topology;

  d3d11_simple_mesh(const d3d11_vertex_buffer<TVertex>& vertex_buffer, D3D11_PRIMITIVE_TOPOLOGY topology) :
    vertex_buffer(vertex_buffer),
    topology(topology)
  { }

  void draw(const com_ptr<ID3D11DeviceContext>& context) const
  {
    vertex_buffer.set(context);
    context->IASetPrimitiveTopology(topology);
    context->Draw(vertex_buffer.size(), 0u);
  }
};

namespace primitives
{
  d3d11_simple_mesh<d3d11_vertex_position_texture> make_quad(const com_ptr<ID3D11Device>& device, float size = 2.f)
  {
    size = size / 2.f;
    array<d3d11_vertex_position_texture, 4> vertices = {
      d3d11_vertex_position_texture{{ -size, size, 0.f }, { 0.f, 0.f }},
      d3d11_vertex_position_texture{{ -size, -size, 0.f }, { 0.f, 1.f }},
      d3d11_vertex_position_texture{{ size, size, 0.f }, { 1.f, 0.f }},
      d3d11_vertex_position_texture{{ size, -size, 0.f }, { 1.f, 1.f }}
    };

    return d3d11_simple_mesh<d3d11_vertex_position_texture>(
      d3d11_vertex_buffer<d3d11_vertex_position_texture>::make_immutable(device, vertices),
      D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
  }
}

std::vector<uint8_t> load_file(const std::filesystem::path& path)
{
  FILE* file = nullptr;
  _wfopen_s(&file, path.c_str(), L"rb");
  fseek(file, 0, SEEK_END);
  auto length = ftell(file);

  std::vector<uint8_t> buffer(length);

  fseek(file, 0, SEEK_SET);
  fread_s(buffer.data(), length, length, 1, file);
  fclose(file);

  return buffer;
}

struct d3d11_vertex_shader
{
private:
  pair<const D3D11_INPUT_ELEMENT_DESC*, ID3D11InputLayout*> _currentLayout = {};
  map<const D3D11_INPUT_ELEMENT_DESC*, com_ptr<ID3D11InputLayout>> _inputLayouts;
  const vector<uint8_t> _source;

  static com_ptr<ID3D11VertexShader> load_shader(const com_ptr<ID3D11Device>& device, const vector<uint8_t>& source)
  {
    com_ptr<ID3D11VertexShader> result;
    check_hresult(device->CreateVertexShader(source.data(), source.size(), nullptr, result.put()));
    return result;
  }

public:
  const com_ptr<ID3D11VertexShader> shader;

  d3d11_vertex_shader(const com_ptr<ID3D11Device>& device, const filesystem::path& path) :
    _source(load_file(path)),
    shader(load_shader(device, _source))
  { }

  void set_input_layout(const array_view<D3D11_INPUT_ELEMENT_DESC>& inputDesc)
  {
    if (_currentLayout.first == inputDesc.data()) return;

    _currentLayout.first = inputDesc.data();

    auto& layout = _inputLayouts[inputDesc.data()];
    if (!layout)
    {
      com_ptr<ID3D11Device> device;
      shader->GetDevice(device.put());

      check_hresult(device->CreateInputLayout(
        inputDesc.data(),
        inputDesc.size(),
        _source.data(), _source.size(), layout.put()));
    }
    _currentLayout.second = layout.get();
  }

  void set(const com_ptr<ID3D11DeviceContext>& context) const
  {
    context->VSSetShader(shader.get(), 0, 0);
    context->IASetInputLayout(_currentLayout.second);
  }
};

struct d3d11_pixel_shader
{
private:
  static com_ptr<ID3D11PixelShader> load_shader(const com_ptr<ID3D11Device>& device, const vector<uint8_t>& source)
  {
    com_ptr<ID3D11PixelShader> result;
    check_hresult(device->CreatePixelShader(source.data(), source.size(), nullptr, result.put()));
    return result;
  }

public:
  const com_ptr<ID3D11PixelShader> shader;

  d3d11_pixel_shader(const com_ptr<ID3D11Device>& device, const filesystem::path& path) :
    shader(load_shader(device, load_file(path)))
  { }

  void set(const com_ptr<ID3D11DeviceContext>& context) const
  {
    context->PSSetShader(shader.get(), 0, 0);
  }
};

struct d3d11_compute_shader
{
private:
  static com_ptr<ID3D11ComputeShader> load_shader(const com_ptr<ID3D11Device>& device, const vector<uint8_t>& source)
  {
    com_ptr<ID3D11ComputeShader> result;
    check_hresult(device->CreateComputeShader(source.data(), source.size(), nullptr, result.put()));
    return result;
  }

public:
  const com_ptr<ID3D11ComputeShader> shader;

  d3d11_compute_shader(const com_ptr<ID3D11Device>& device, const filesystem::path& path) :
    shader(load_shader(device, load_file(path)))
  { }

  void run(const com_ptr<ID3D11DeviceContext>& context, uint32_t x, uint32_t y, uint32_t z) const
  {
    context->CSSetShader(shader.get(), 0, 0);
    context->Dispatch(x, y, z);
  }
};

enum class d3d11_rasterizer_type
{
  cull_none,
  cull_clockwise,
  cull_counter_clockwise,
  wireframe
};

struct d3d11_rasterizer_state
{
private:
  static com_ptr<ID3D11RasterizerState> make_state(const com_ptr<ID3D11Device>& device, d3d11_rasterizer_type type)
  {
    CD3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
    switch (type)
    {
    case d3d11_rasterizer_type::cull_none:
      rasterizerDesc.CullMode = D3D11_CULL_NONE;
      break;
    case d3d11_rasterizer_type::cull_clockwise:
      rasterizerDesc.CullMode = D3D11_CULL_FRONT;
      break;
    case d3d11_rasterizer_type::cull_counter_clockwise:
      rasterizerDesc.CullMode = D3D11_CULL_BACK;
      break;
    case d3d11_rasterizer_type::wireframe:
      rasterizerDesc.CullMode = D3D11_CULL_NONE;
      rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
      break;
    }

    com_ptr<ID3D11RasterizerState> result;
    check_hresult(device->CreateRasterizerState(&rasterizerDesc, result.put()));
    return result;
  }

public:
  const com_ptr<ID3D11RasterizerState> state;

  d3d11_rasterizer_state(const com_ptr<ID3D11Device>& device, d3d11_rasterizer_type type) :
    state(make_state(device, type))
  { }

  void set(const com_ptr<ID3D11DeviceContext>& context) const
  {
    context->RSSetState(state.get());
  }
};

enum class d3d11_blend_type
{
  opaque,
  additive,
  subtractive,
  alpha_blend
};

struct d3d11_blend_state
{
private:
  static com_ptr<ID3D11BlendState> make_state(const com_ptr<ID3D11Device>& device, d3d11_blend_type type)
  {
    D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
    renderTargetBlendDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    switch (type)
    {
    case d3d11_blend_type::opaque:
      renderTargetBlendDesc.BlendEnable = false;
      renderTargetBlendDesc.SrcBlend = D3D11_BLEND_ONE;
      renderTargetBlendDesc.DestBlend = D3D11_BLEND_ZERO;
      renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
      renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
      renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
      renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
      break;
    case d3d11_blend_type::additive:
    case d3d11_blend_type::subtractive:
      renderTargetBlendDesc.BlendEnable = true;
      renderTargetBlendDesc.SrcBlend = D3D11_BLEND_ONE;
      renderTargetBlendDesc.DestBlend = D3D11_BLEND_ONE;
      renderTargetBlendDesc.BlendOp = renderTargetBlendDesc.BlendOpAlpha = (type == d3d11_blend_type::additive ? D3D11_BLEND_OP_ADD : D3D11_BLEND_OP_REV_SUBTRACT);
      renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
      renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_ONE;
      renderTargetBlendDesc.BlendOpAlpha = renderTargetBlendDesc.BlendOp;
      break;
    case d3d11_blend_type::alpha_blend:
      renderTargetBlendDesc.BlendEnable = true;
      renderTargetBlendDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
      renderTargetBlendDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
      renderTargetBlendDesc.BlendOp = D3D11_BLEND_OP_ADD;
      renderTargetBlendDesc.SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
      renderTargetBlendDesc.DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
      renderTargetBlendDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
      break;
    }

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0] = renderTargetBlendDesc;

    com_ptr<ID3D11BlendState> result;
    winrt::check_hresult(device->CreateBlendState(&blendDesc, result.put()));
    return result;
  }

public:
  const com_ptr<ID3D11BlendState> state;

  d3d11_blend_state(const com_ptr<ID3D11Device>& device, d3d11_blend_type type) :
    state(make_state(device, type))
  { }

  void set(const com_ptr<ID3D11DeviceContext>& context) const
  {
    context->OMSetBlendState(state.get(), nullptr, 0xffffffff);
  }
};

struct d3d11_sampler_state
{
private:
  static com_ptr<ID3D11SamplerState> make_state(const com_ptr<ID3D11Device>& device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode)
  {
    float borderColor[4] = { 0.f, 0.f, 0.f, 0.f };
    CD3D11_SAMPLER_DESC sd(
      filter,
      addressMode,
      addressMode,
      addressMode,
      0.f,
      0,
      D3D11_COMPARISON_NEVER,
      borderColor,
      0.f,
      D3D11_FLOAT32_MAX
    );

    com_ptr<ID3D11SamplerState> state;
    check_hresult(device->CreateSamplerState(&sd, state.put()));
    return state;
  }

public:
  const com_ptr<ID3D11SamplerState> state;

  d3d11_sampler_state(const com_ptr<ID3D11Device>& device, D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode) :
    state(make_state(device, filter, addressMode))
  { }

  void set(const com_ptr<ID3D11DeviceContext>& context, d3d11_shader_stage stage, int slot = 0) const
  {
    const array<ID3D11SamplerState*, 1> samplerStates = { state.get() };

    switch (stage)
    {
    case d3d11_shader_stage::cs:
      context->CSSetSamplers(slot, 1, samplerStates.data());
      break;
    case d3d11_shader_stage::vs:
      context->VSSetSamplers(slot, 1, samplerStates.data());
      break;
    case d3d11_shader_stage::ps:
      context->PSSetSamplers(slot, 1, samplerStates.data());
      break;
    default:
      throw out_of_range("Invalid shader stage for texture!");
    }
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

struct d3d11_desktop_duplication
{
private:  
  com_ptr<IDXGIOutputDuplication> _outputDuplication;
  unique_ptr<d3d11_texture_2d> _texture;

public:
  const com_ptr<ID3D11Device> device;
  const com_ptr<IDXGIOutput2> output;

  d3d11_desktop_duplication(const com_ptr<ID3D11Device>& device, const com_ptr<IDXGIOutput2>& output) :
    device(device),
    output(output)
  { }

  d3d11_texture_2d& lock_frame(uint16_t timeout = 1000u)
  {
    com_ptr<IDXGIResource> resource;
    HRESULT result;
    do
    {
      if (_outputDuplication == nullptr)
      {
        output->DuplicateOutput(device.get(), _outputDuplication.put());
      }

      if (_outputDuplication != nullptr)
      {
        DXGI_OUTDUPL_FRAME_INFO frameInfo;
        auto result = _outputDuplication->AcquireNextFrame(timeout, &frameInfo, resource.put());
        if (result != ERROR_SUCCESS)
        {
          _outputDuplication = nullptr;
        }
      }
      else
      {
        Sleep(100);
      }
    } while (!resource);

    auto texture = resource.as<ID3D11Texture2D>();
    if (!_texture || _texture->texture != texture)
    {
      _texture = make_unique<d3d11_texture_2d>(texture);
    }

    return *_texture;
  }

  void unlock_frame()
  {
    _outputDuplication->ReleaseFrame();
  }
};

filesystem::path get_root()
{
  wchar_t executablePath[MAX_PATH];
  GetModuleFileNameW(NULL, executablePath, MAX_PATH);

  return filesystem::path(executablePath).remove_filename();
}

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::AppService;

int main()
{
  init_apartment();

  /*AppServiceConnection connection;
  connection.AppServiceName(L"CommunicationService");
  connection.PackageFamilyName(Package::Current().Id().FamilyName());
  auto receivedRevoker = connection.RequestReceived(auto_revoke, [](auto&, const AppServiceRequestReceivedEventArgs& eventArgs) {
    printf("Message received.");
  });

  auto status = connection.OpenAsync().get();
  switch (status)
  {
  case AppServiceConnectionStatus::Success:
    printf("Connection established - waiting for requests");
    break;
  case AppServiceConnectionStatus::AppNotInstalled:
    printf("The app AppServicesProvider is not installed.");
    break;
  case AppServiceConnectionStatus::AppUnavailable:
    printf("The app AppServicesProvider is not available.");
    break;
  case AppServiceConnectionStatus::AppServiceUnavailable:
    wprintf(L"The app AppServicesProvider is installed but it does not provide the app service %s.", connection.AppServiceName().c_str());
    break;
  case AppServiceConnectionStatus::Unknown:
    printf("An unkown error occurred while we were trying to open an AppServiceConnection.");
    break;
  }

  Sleep(INFINITE);
  return 0;*/
  auto root = get_root();

  auto output = get_default_output();

  DXGI_OUTPUT_DESC1 desc;
  output.as<IDXGIOutput6>()->GetDesc1(&desc);

  com_ptr<IDXGIAdapter> adapter;
  check_hresult(output->GetParent(__uuidof(IDXGIAdapter), adapter.put_void()));

  MONITORINFOEX x;
  x.cbSize = sizeof(MONITORINFOEX);
  
  GetMonitorInfo(desc.Monitor, &x);

  DISPLAY_DEVICE dev = {};
  dev.cb = sizeof(dev);
  EnumDisplayDevices(desc.DeviceName, 0, &dev, 0);

  auto window = create_debug_window();

  d3d11_renderer_with_swap_chain renderer(adapter, window);

  auto duplication = d3d11_desktop_duplication(renderer.device, output);

  auto vertexShader = d3d11_vertex_shader(renderer.device, root / L"SimpleVertexShader.cso");
  auto pixelShader = d3d11_pixel_shader(renderer.device, root / L"SimplePixelShader.cso");
  
  auto quad = primitives::make_quad(renderer.device);
  vertexShader.set_input_layout(quad.vertex_buffer.input_desc());

  auto sampler = d3d11_sampler_state(renderer.device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP);
  auto blendState = d3d11_blend_state(renderer.device, d3d11_blend_type::opaque);

  auto rasterizerState = d3d11_rasterizer_state(renderer.device, d3d11_rasterizer_type::cull_none);

  while (true)
  {
    auto& texture = duplication.lock_frame();

    auto& target = renderer.render_target();
    target.set(renderer.context);
    target.clear(renderer.context, { 1.f, 0.f, 0.f, 1.f });

    vertexShader.set(renderer.context);
    pixelShader.set(renderer.context);
    texture.set(renderer.context, d3d11_shader_stage::ps);
    sampler.set(renderer.context, d3d11_shader_stage::ps);
    blendState.set(renderer.context);
    rasterizerState.set(renderer.context);
    quad.draw(renderer.context);



    renderer.swap_chain->Present(1, 0);

    duplication.unlock_frame();
  }
}
