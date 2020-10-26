#include "pch.h"
#include "Openable.h"

using namespace std;

namespace Lux::Infrastructure
{
  bool openable::is_open() const
  {
    return _is_open;
  }

  void openable::open()
  {
    if (_is_open) return;

    _is_open = true;
    on_opening();
  }
}
