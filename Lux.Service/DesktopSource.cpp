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

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation::Numerics;


namespace Lux::Sources
{
  DesktopSource::~DesktopSource()
  {
    _isWorkerActive = false;
    _workerThread.reset();
  }

  LightSourceKind DesktopSource::Kind()
  {
    return LightSourceKind::Desktop;
  }

  std::pair<std::vector<Math::rect>, std::vector<std::vector<std::pair<uint16_t, float>>>> DesktopSource::CalculateSamplingRegions(const DisplaySettings& settings)
  {
    auto horizontalDivisions = size_t(_verticalDivisions * settings.AspectRatio);
    float2 sampleSize{ _sampleSize / settings.AspectRatio, _sampleSize };

    //Define light sampling areas
    vector<rect> lightRects;
    lightRects.reserve(settings.SamplePoints.size());
    auto sampleOffset = sampleSize / float2(2.f, -2.f);
    for (auto& samplePoint : settings.SamplePoints)
    {
      lightRects.push_back({ samplePoint - sampleOffset, samplePoint + sampleOffset });
    }

    //Allocata screen samples to lights
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

  void DesktopSource::OnSettingsChanged()
  {
    _isWorkerActive = false;
    _workerThread.reset();

    _isWorkerActive = true;
    _workerThread = make_unique<background_thread>(member_func(this, &DesktopSource::Capture), L"desktop capture");
  }

  void DesktopSource::Capture()
  {
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
    vector<rgb> currentColors(factors.size());

    while (_isWorkerActive)
    {
      auto texture = duplication.lock_frame(17ms);
      if (texture)
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
          float4 color{};
          for (auto& [cell, factor] : rectFactors)
          {
            auto& sampleU = *((led_color_t*)data.data() + cell);
            auto sampleF = float3((float)sampleU[0], (float)sampleU[1], (float)sampleU[2]);
            color += float4(sampleF * factor, factor);
          }

          rgb newColor{ uint8_t(color.x / color.w), uint8_t(color.y / color.w), uint8_t(color.z / color.w) };
          *it++ = newColor;
        }

        duplication.unlock_frame();
      }
      else
      {
        this_thread::sleep_for(100ms);
      }

      auto it = currentColors.begin();
      for (auto& color : targetColors)
      {
        *it++ = lerp(*it, color, 0.2f);
      }

      EmitColors(vector<rgb>{ currentColors });
    }
  }
}