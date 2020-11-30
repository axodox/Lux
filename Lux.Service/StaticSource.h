#pragma once
#include "LightSource.h"
#include "BackgroundThread.h"

namespace Lux::Sources
{
  class StaticSource : public LightSource
  {
  public:
    StaticSource();

    Graphics::rgb Color() const;
    void Color(Graphics::rgb value);

    virtual Configuration::LightSourceKind Kind() override;

  private:
    Graphics::rgb _color = { 255, 255, 255 };
    Threading::background_thread _worker;

    void Worker();
  };
}