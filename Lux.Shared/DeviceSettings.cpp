#include "pch.h"
#include "DeviceSettings.h"

using namespace Lux::Json;
using namespace std::chrono;
using namespace winrt::Windows::Foundation::Numerics;

namespace Lux::Configuration
{
  DisplaySize::DisplaySize(const Json::json_value& value)
  {
    json_object object{ value };
    object.read(L"width", Width);
    object.read(L"height", Height);
  }

  float2 DisplayPosition::ToAbsolutePosition(const DisplaySize& size) const
  {
    float2 position;
    switch (Reference)
    {
    case DisplayPositionReference::BottomLeft:
      position = { X, Y };
      break;
    case DisplayPositionReference::BottomRight:
      position = { size.Width - X, Y };
      break;
    case DisplayPositionReference::TopLeft:
      position = { X, size.Height - Y };
      break;
    case DisplayPositionReference::TopRight:
      position = { size.Width - X, size.Height - Y };
      break;
    default:
      position = {};
      break;
    }

    return position;
  }

  DisplayPosition::DisplayPosition(const Json::json_value& value)
  {
    json_object object{ value };
    object.read(L"reference", Reference);
    object.read(L"x", X);
    object.read(L"y", Y);
  }

  DisplayLightStrip::DisplayLightStrip(const Json::json_value& value)
  {
    json_object object{ value };
    object.read(L"endPosition", EndPosition);
    object.read(L"lightCount", LightCount);
  }

  AdaLightSettings::AdaLightSettings(const Json::json_value& value)
  {
    json_object object{ value };
    object.read(L"usbVendorId", UsbVendorId, 0x1A86ui16);
    object.read(L"usbProductId", UsbProductId, 0x7523ui16);
    object.read(L"baudRate", BaudRate, 1000000u);
    LedSyncDuration = milliseconds{ object.read_or(L"ledSyncDuration", 10) };
  }

  DisplayLightLayout::DisplayLightLayout(const Json::json_value& value)
  {
    json_object object{ value };
    object.read(L"displaySize", DisplaySize);
    object.read(L"startPosition", StartPosition);
    object.read(L"segments", Segments);
  }

  void DisplayLightLayout::serialize(Serialization::stream& stream) const
  {
    stream.write(DisplaySize);
    stream.write(StartPosition);
    stream.write(Segments);
  }

  void DisplayLightLayout::deserialize(Serialization::stream& stream)
  {
    stream.read(DisplaySize);
    stream.read(StartPosition);
    stream.read(Segments);
  }
  
  uint32_t DisplayLightLayout::LedCount() const
  {
    auto count = 0u;
    for (auto& segment : Segments)
    {
      count += segment.LightCount;
    }
    return count;
  }

  DeviceSettings::DeviceSettings(const Json::json_value& value)
  {
    json_object object{ value };
    object.read(L"adaLight", AdaLight);
    object.read(L"layout", Layout);
  }

  void DeviceSettings::serialize(Serialization::stream& stream) const
  {
    stream.write(AdaLight);
    stream.write(Layout);
  }

  void DeviceSettings::deserialize(Serialization::stream& stream)
  {
    stream.read(AdaLight);
    stream.read(Layout);
  }
}