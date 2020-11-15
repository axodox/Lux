#pragma once
#include "LightSource.h"

namespace Lux::Sources
{
  class StaticSource : public LightSource
  {
  public:
    StaticSource();
    virtual ~StaticSource();

    Graphics::rgb Color() const;
    void Color(Graphics::rgb value);

    virtual Configuration::LightSourceKind Kind() override;

  private:
    winrt::Windows::System::Threading::ThreadPoolTimer _timer;
    Graphics::rgb _color = { 255, 255, 255 };

    void OnTick(const winrt::Windows::System::Threading::ThreadPoolTimer& timer);
  };
}