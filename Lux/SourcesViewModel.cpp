#include "pch.h"
#include "SourcesViewModel.h"
#include "SourcesViewModel.g.cpp"

#include "DependencyConfiguration.h"
#include "ObservableClient.h"
#include "BitwiseOperators.h"

using namespace ::Lux;
using namespace ::Lux::BitwiseOperations;
using namespace ::Lux::Configuration;
using namespace ::Lux::Events;
using namespace ::Lux::Graphics;
using namespace ::Lux::Observable;
using namespace ::Lux::Threading;

using namespace winrt;
using namespace winrt::Windows::UI::Xaml::Data;
using namespace winrt::Windows::UI::Xaml::Input;

namespace winrt::Lux::implementation
{
  SourcesViewModel::SourcesViewModel() :
    _client(DependencyConfiguration::Instance().resolve<observable_client<LightConfiguration>>()),
    _dispatcher(DependencyConfiguration::Instance().resolve<dispatcher>())
  {
    _client->full_data_reset(no_revoke, member_func(this, &SourcesViewModel::OnFullDataReset));
    _client->root()->property_changed(no_revoke, member_func(this, &SourcesViewModel::OnSettingChanged));
  }

  SourceKind SourcesViewModel::Source()
  {
    return (SourceKind)*_client->root()->LightSource;
  }

  void SourcesViewModel::Source(SourceKind value)
  {
    _client->root()->LightSource = (LightSourceKind)value;
  }

  Windows::UI::Color SourcesViewModel::StaticSourceColor()
  {
    return rgb(_client->root()->StaticSourceOptions->Color);
  }

  void SourcesViewModel::StaticSourceColor(Windows::UI::Color color)
  {
    if (are_equal(*_client->root()->StaticSourceOptions->Color, rgb(color))) return;
    _client->root()->StaticSourceOptions->Color = rgb(color);
  }

  uint8_t SourcesViewModel::RainbowSourceSpatialFrequency()
  {
    return _client->root()->RainbowSourceOptions->SpatialFrequency;
  }

  void SourcesViewModel::RainbowSourceSpatialFrequency(uint8_t value)
  {
    if (_client->root()->RainbowSourceOptions->SpatialFrequency == value) return;
    _client->root()->RainbowSourceOptions->SpatialFrequency = value;
  }

  float SourcesViewModel::RainbowSourceAngularVelocity()
  {
    return _client->root()->RainbowSourceOptions->AngularVelocity / 2.f / float(M_PI);
  }

  void SourcesViewModel::RainbowSourceAngularVelocity(float value)
  {
    value = value * 2.f * float(M_PI);

    if (_client->root()->RainbowSourceOptions->AngularVelocity == value) return;
    _client->root()->RainbowSourceOptions->AngularVelocity = value;
  }

  float SourcesViewModel::DesktopSourceTemporalAveraging()
  {
    return _client->root()->DesktopSourceOptions->TemporalAveraging;
  }

  void SourcesViewModel::DesktopSourceTemporalAveraging(float value)
  {
    if (_client->root()->DesktopSourceOptions->TemporalAveraging == value) return;
    _client->root()->DesktopSourceOptions->TemporalAveraging = value;
  }

  float SourcesViewModel::DesktopSourceSampleSize()
  {
    return _client->root()->DesktopSourceOptions->SampleSize;
  }

  void SourcesViewModel::DesktopSourceSampleSize(float value)
  {
    if (_client->root()->DesktopSourceOptions->SampleSize == value) return;
    _client->root()->DesktopSourceOptions->SampleSize = value;
  }

  event_token SourcesViewModel::PropertyChanged(PropertyChangedEventHandler const& value)
  {
    return _propertyChanged.add(value);
  }

  void SourcesViewModel::PropertyChanged(event_token const& token)
  {
    _propertyChanged.remove(token);
  }

  void SourcesViewModel::OnFullDataReset(observable_client<LightConfiguration>* /*sender*/)
  {
    _dispatcher->invoke([&] {
      _propertyChanged(*this, PropertyChangedEventArgs(L""));
      });
  }

  void SourcesViewModel::OnSettingChanged(observable_object<LightConfigurationProperty>* /*object*/, LightConfigurationProperty propertyKey)
  {
    switch (propertyKey)
    {
    case LightConfigurationProperty::LightSource:
      _propertyChanged(*this, PropertyChangedEventArgs(L"Source"));
      break;
    case LightConfigurationProperty::StaticSourceOptions:
      _propertyChanged(*this, PropertyChangedEventArgs(L"StaticSourceColor"));
      break;
    case LightConfigurationProperty::RainbowSourceOptions:
      _propertyChanged(*this, PropertyChangedEventArgs(L"RainbowSourceSpatialFrequency"));
      _propertyChanged(*this, PropertyChangedEventArgs(L"RainbowSourceAngularVelocity"));
      break;
    case LightConfigurationProperty::DesktopSourceOptions:
      _propertyChanged(*this, PropertyChangedEventArgs(L"DesktopSourceTemporalAveraging"));
      _propertyChanged(*this, PropertyChangedEventArgs(L"DesktopSourceSampleSize"));
      break;
    }
  }
}
