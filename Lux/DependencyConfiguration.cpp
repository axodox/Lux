#include "pch.h"
#include "DependencyConfiguration.h"
#include "TcpMessagingClient.h"
#include "NetworkSettings.h"
#include "ObservableClient.h"
#include "LightConfiguration.h"

using namespace Lux::Configuration;
using namespace Lux::Infrastructure;
using namespace Lux::Observable;
using namespace Lux::Networking;
using namespace Lux::Threading;
using namespace std;

namespace Lux
{
  DependencyConfiguration::DependencyConfiguration()
  {
    _container.add<dispatcher>(dependency_lifetime::singleton, [] {
      return make_unique<core_dispatcher>(winrt::Windows::UI::Xaml::Window::Current().Dispatcher());
      });

    _container.add<observable_client<LightConfiguration>>(dependency_lifetime::singleton, [&] {
      return make_unique<observable_client<LightConfiguration>>(
        make_unique<tcp_messaging_client>(ip_endpoint{ L"127.0.0.1", LuxPort }),
        _container.resolve<dispatcher>()
        );
      });
  }

  Infrastructure::dependency_container& DependencyConfiguration::Instance()
  {
    static unordered_map<uint32_t, unique_ptr<DependencyConfiguration>> configurations;
    auto& configuration = configurations[GetCurrentThreadId()];
    if (!configuration)
    {
      configuration = unique_ptr<DependencyConfiguration>{ new DependencyConfiguration() };
    }
    return configuration->_container;
  }
}