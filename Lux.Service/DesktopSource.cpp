#include "pch.h"
#include "DesktopSource.h"

#include "IO.h"

#include "DXGIOutput.h"
#include "D3D11Renderer.h"
#include "D3D11DesktopDuplication.h"
#include "D3D11SamplerState.h"
#include "D3D11StructuredBuffer.h"
#include "D3D11ComputeShader.h"
#include "D3D11ConstantBuffer.h"

using namespace Lux::IO;
using namespace Lux::Math;
using namespace Lux::Configuration;
using namespace Lux::Events;
using namespace Lux::Graphics;
using namespace Lux::Threading;

using namespace DirectX;
using namespace std;
using namespace std::chrono;
using namespace std::chrono_literals;
using namespace winrt;
using namespace winrt::Windows::Foundation::Numerics;

namespace Lux::Sources
{
  DesktopSource::DesktopSource() :
    _restartAggregator(member_func(this, &DesktopSource::RestartCapture), 100ms, L"desktop capture restarter")
  { }

  LightSourceKind DesktopSource::Kind()
  {
    return LightSourceKind::Desktop;
  }

  float DesktopSource::TemporalAveraging() const
  {
    return _temporalAveraging;
  }

  void DesktopSource::TemporalAveraging(float value)
  {
    _temporalAveraging = value;
  }

  float DesktopSource::SampleSize() const
  {
    return _sampleSize;
  }

  void DesktopSource::SampleSize(float value)
  {
    if (_sampleSize == value) return;

    _sampleSize = value;
    _restartAggregator.call();
  }

  std::pair<std::vector<Math::rect>, std::vector<std::vector<std::pair<uint16_t, float>>>> DesktopSource::CalculateSamplingRegions(const DisplaySettings& settings)
  {
    auto horizontalDivisions = size_t(_verticalDivisions * (isnan(settings.AspectRatio) ? 1 : settings.AspectRatio));
    float2 sampleSize{ _sampleSize / settings.AspectRatio, _sampleSize };

    //Define light sampling areas
    vector<rect> lightRects;
    lightRects.reserve(settings.SamplePoints.size());
    auto sampleOffset = sampleSize / float2(2.f, -2.f);
    for (auto& samplePoint : settings.SamplePoints)
    {
      lightRects.push_back({ samplePoint - sampleOffset, samplePoint + sampleOffset });
    }

    //Allocate screen samples to lights
    vector<vector<pair<uint16_t, float>>> lightsDisplayRectFactors(settings.SamplePoints.size());

    vector<rect> displayRects;
    displayRects.reserve(_verticalDivisions * horizontalDivisions);

    float2 step{ 1.f / horizontalDivisions, 1.f / _verticalDivisions };
    rect displayRect{ step.y, 0, 0, step.x };
    for (size_t y = 0u; y < _verticalDivisions; y++)
    {
      displayRect.left = 0;
      displayRect.right = step.x;

      for (size_t x = 0u; x < horizontalDivisions; x++)
      {
        uint16_t lightIndex = 0u;
        bool isUsed = false;
        for (auto& sampleRect : lightRects)
        {
          if (sampleRect.intersects(displayRect))
          {
            isUsed = true;

            auto distance = length((displayRect.center() - sampleRect.center()) / 2.f / sampleSize);
            auto weight = 1.f - distance;
            lightsDisplayRectFactors[lightIndex].push_back({ (uint16_t)displayRects.size(), weight });
          }

          lightIndex++;
        }

        if (isUsed)
        {
          displayRects.push_back(displayRect);
        }

        displayRect.left += step.x;
        displayRect.right += step.x;
      }

      displayRect.top += step.y;
      displayRect.bottom += step.y;
    }

    //Calculate sample weights
    for (auto& lightDisplayRectFactors : lightsDisplayRectFactors)
    {
      auto totalWeight = 0.f;
      for (auto& displayRectFactor : lightDisplayRectFactors)
      {
        totalWeight += displayRectFactor.second;
      }

      for (auto& displayRectFactor : lightDisplayRectFactors)
      {
        displayRectFactor.second /= totalWeight;
      }
    }

    return pair{ move(displayRects), move(lightsDisplayRectFactors) };
  }


  void DesktopSource::RestartCapture()
  {
    _workerThread.reset();
    _workerThread = make_unique<background_thread>(member_func(this, &DesktopSource::Capture), L"desktop capture");
  }

  void DesktopSource::OnSettingsChanged()
  {
    _restartAggregator.call();
  }

  void DesktopSource::Capture()
  {
    auto worker = _workerThread.get();
    auto output = get_default_output();
    auto adapter = get_adapter(output);

    d3d11_renderer renderer{ adapter };
    d3d11_desktop_duplication duplication{ renderer.device(), output };
    d3d11_sampler_state sampler{ renderer.device(), D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP };

    vector<rect> rects;
    vector<vector<pair<uint16_t, float>>> factors;

    {
      auto settings = Settings();
      tie(rects, factors) = CalculateSamplingRegions(*settings);
    }  

    typedef array<uint32_t, 4> led_color_t;
    auto samplePoints = d3d11_structured_buffer::make_immutable(renderer.device(), rects);
    auto sampledColorSums = d3d11_structured_buffer::make_writeable<led_color_t>(renderer.device(), uint32_t(rects.size()));
    auto sampledColorStage = d3d11_structured_buffer::make_staging<led_color_t>(renderer.device(), uint32_t(rects.size()));
    auto samplerShader = d3d11_compute_shader(renderer.device(), app_folder() / L"DesktopSamplingComputeShader.cso");
    auto constantsBuffer = d3d11_constant_buffer::make_dynamic<bool>(renderer.device());

    vector<rgb> targetColors(factors.size());
    vector<XMFLOAT3> currentColors(factors.size());

    while (!worker->is_shutting_down())
    {
      d3d11_texture_2d* texture;
      auto state = duplication.try_lock_frame(17ms, texture);
      if (state == d3d11_desktop_duplication_state::ready)
      {
        constantsBuffer.write(renderer.context(), duplication.is_hdr());
        constantsBuffer.set(renderer.context(), d3d11_shader_stage::cs);

        sampler.set(renderer.context(), d3d11_shader_stage::cs);
        texture->set(renderer.context(), d3d11_shader_stage::cs);

        samplePoints.set_readonly(renderer.context(), 1);
        sampledColorSums.set_writeable(renderer.context());
        samplerShader.run(renderer.context(), uint32_t(rects.size()));
        sampledColorSums.copy_to(renderer.context(), sampledColorStage);

        auto data = sampledColorStage.read(renderer.context());
        auto it = targetColors.begin();
        for (auto rectFactors : factors)
        {
          XMVECTOR avgColor{};
          for (auto& [cell, factor] : rectFactors)
          {
            auto& sampleU = *((led_color_t*)data.data() + cell);
            auto sampleF = XMVectorSet((float)sampleU[0], (float)sampleU[1], (float)sampleU[2], 1.f);
            avgColor = XMVectorAdd(avgColor, XMVectorScale(sampleF, factor));
          }

          avgColor = XMVectorScale(avgColor, 1.f / XMVectorGetW(avgColor));

          XMFLOAT3 color;
          XMStoreFloat3(&color, avgColor);

          rgb newColor{ uint8_t(color.x), uint8_t(color.y), uint8_t(color.z) };
          *it++ = newColor;
        }

        duplication.unlock_frame();
      }
      else if(state != d3d11_desktop_duplication_state::timeout)
      {
        this_thread::sleep_for(100ms);
      }

      if (_temporalAveraging > 0)
      {
        auto sourceIt = currentColors.begin();

        auto factor = max(1.f - _temporalAveraging, 0.01f);
        auto invFactor = 1.f - factor;

        vector<rgb> results;
        results.reserve(currentColors.size());
        for (auto& color : targetColors)
        {
          auto& sourceColor = *sourceIt++;

          auto source = XMLoadFloat3(&sourceColor);
          auto target = XMVectorSet(color.r, color.g, color.b, 0);

          auto blend = XMVectorAdd(XMVectorScale(source, invFactor), XMVectorScale(target, factor));

          XMStoreFloat3(&sourceColor, blend);
          results.push_back({ uint8_t(sourceColor.x), uint8_t(sourceColor.y), uint8_t(sourceColor.z) });
        }
        EmitColors(move(results));
      }
      else
      {
        EmitColors(vector<rgb>{ targetColors });
      }      
    }
  }
}