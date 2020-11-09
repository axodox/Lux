#pragma once
#include "Json.h"
#include "EnumNameMapping.h"
#include "Serializer.h"

namespace Lux::Configuration
{
  struct DisplaySize
  {
    float Width, Height;

    DisplaySize() = default;
    DisplaySize(const Json::json_value& value);
  };

  named_enum_underlying(DisplayPositionReference, uint8_t,
    BottomLeft,
    BottomRight,
    TopLeft,
    TopRight
  );

  struct DisplayPosition
  {
    DisplayPositionReference Reference;
    float X, Y;

    winrt::Windows::Foundation::Numerics::float2 ToAbsolutePosition(const DisplaySize& size) const;

    DisplayPosition() = default;
    DisplayPosition(const Json::json_value& value);
  };

  struct DisplayLightStrip
  {
    DisplayPosition EndPosition;
    uint16_t LightCount;

    DisplayLightStrip() = default;
    DisplayLightStrip(const Json::json_value& value);
  };

  struct AdaLightSettings
  {
    uint16_t UsbVendorId;
    uint16_t UsbProductId;
    uint32_t BaudRate;
    std::chrono::milliseconds LedSyncDuration;

    AdaLightSettings() = default;
    AdaLightSettings(const Json::json_value& value);
  };

  struct DisplayLightLayout : public Serialization::serializable
  {
    DisplaySize DisplaySize;
    DisplayPosition StartPosition;
    std::vector<DisplayLightStrip> Segments;

    DisplayLightLayout() = default;
    DisplayLightLayout(const Json::json_value & value);

    virtual void serialize(Serialization::stream& stream) const;
    virtual void deserialize(Serialization::stream& stream);

    uint32_t LedCount() const;
  };

  struct DeviceSettings : public Serialization::serializable
  {
    AdaLightSettings AdaLight;
    DisplayLightLayout Layout;

    DeviceSettings() = default;
    DeviceSettings(const Json::json_value& value);

    virtual void serialize(Serialization::stream& stream) const;
    virtual void deserialize(Serialization::stream& stream);
  };
}