#include "pch.h"
#include "LightLayout.h"

using namespace winrt::Windows::Foundation::Numerics;

namespace Lux::Configuration
{
  DisplaySize::DisplaySize(const Json::json_value& value)
  {
    Json::json_object object{ value };
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
    Json::json_object object{ value };
    object.read(L"reference", Reference);
    object.read(L"x", X);
    object.read(L"y", Y);
  }

  DisplayLightStrip::DisplayLightStrip(const Json::json_value& value)
  {
    Json::json_object object{ value };
    object.read(L"displayPosition", EndPosition);
    object.read(L"lightCount", LightCount);
  }

  DisplayLightLayout::DisplayLightLayout(const Json::json_value& value)
  {
    Json::json_object object{ value };
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
}