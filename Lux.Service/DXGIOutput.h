#pragma once
#include "pch.h"

namespace Lux::Graphics
{
  winrt::com_ptr<IDXGIOutput5> get_default_output();

  winrt::com_ptr<IDXGIAdapter> get_adapter(const winrt::com_ptr<IDXGIOutput5>& output);
}