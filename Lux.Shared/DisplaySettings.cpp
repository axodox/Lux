#include "pch.h"
#include "DisplaySettings.h"

using namespace std;
using namespace winrt::Windows::Foundation::Numerics;

namespace Lux::Configuration
{
  DisplaySettings DisplaySettings::FromLayout(const DisplayLightLayout& layout)
  {
    DisplaySettings settings{};
    settings.AspectRatio = layout.DisplaySize.Width / layout.DisplaySize.Height;

    auto displaySize = reinterpret_cast<const float2&>(layout.DisplaySize);
    auto displayCenter = displaySize / 2.f;
    auto startPosition = layout.StartPosition.ToAbsolutePosition(layout.DisplaySize);
    for (auto& segment : layout.Segments)
    {
      auto endPosition = segment.EndPosition.ToAbsolutePosition(layout.DisplaySize);

      auto lightDistance = (endPosition - startPosition) / max((float)segment.LightCount - 1.f, 1.f);
      for (auto i = 0u; i < segment.LightCount; i++)
      {
        auto position = startPosition + float(i) * lightDistance;
        settings.SamplePoints.push_back(position / displaySize);
      }

      startPosition = endPosition;
    }

    return settings;
  }
}