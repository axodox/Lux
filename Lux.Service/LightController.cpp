#include "pch.h"
#include "LightController.h"

namespace Lux::Controllers
{
  LightController::LightController() :
    IsConnectedChanged(_events)
  { }

  bool LightController::IsConnected()
  {
    return _isConnected;
  }

  void LightController::IsConnected(bool value)
  {
    _isConnected = value;
    _events.raise(IsConnectedChanged, this);
  }
}
