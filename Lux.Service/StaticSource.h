#pragma once
#include "pch.h"
#include "LightSource.h"

namespace Lux::Sources
{
  class StaticSource : public LightSource
  {
  public:
    StaticSource();

    Graphics::rgb Color() const;
    void Color(Graphics::rgb value);

  private:
    winrt::Windows::System::Threading::ThreadPoolTimer _timer;
    Graphics::rgb _color = { 255, 255, 255 };

    void OnTick(const winrt::Windows::System::Threading::ThreadPoolTimer& timer);
  };
}