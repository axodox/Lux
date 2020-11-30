#pragma once
#include "Colors.h"
#include "Events.h"

namespace Lux::Controllers
{
  class LightController
  {
  private:
    Events::event_owner _events;
    bool _isConnected = false;

  protected:
    void IsConnected(bool value);

  public:
    Events::event_publisher<LightController*> IsConnectedChanged;

    LightController();
    virtual ~LightController() = default;

    bool IsConnected();
    virtual void Push(const std::vector<Graphics::rgb>& colors) = 0;
  };
}