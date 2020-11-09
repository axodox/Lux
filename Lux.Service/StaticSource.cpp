#include "pch.h"
#include "StaticSource.h"

using namespace Lux::Graphics;

using namespace std;
using namespace std::chrono_literals;

using namespace winrt::Windows::System::Threading;

namespace Lux::Sources
{
  StaticSource::StaticSource() :
    _timer(ThreadPoolTimer::CreatePeriodicTimer({ this, &StaticSource::OnTick}, 16ms))
  { }

  Graphics::rgb StaticSource::Color() const
  {
    return _color;
  }

  void StaticSource::Color(Graphics::rgb value)
  {
    _color = value;
  }

  void StaticSource::OnTick(const winrt::Windows::System::Threading::ThreadPoolTimer& /*timer*/)
  {
    auto settings = Settings();
    if (!settings) return;

    auto count = settings->SamplePoints.size();    
    if (count == 0u) return;

    EmitColors({ count, _color });
  }
}