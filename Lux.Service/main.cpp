#include "pch.h"
#include "Configuration.h"
#include "TcpMessagingServer.h"
#include "Dispatcher.h"
#include "ObservableServer.h"

using namespace Lux;
using namespace std;
using namespace winrt;

int WINAPI wWinMain(
  _In_ HINSTANCE /*hInstance*/,
  _In_opt_ HINSTANCE /*hPrevInstance*/,
  _In_ LPWSTR /*lpCmdLine*/,
  _In_ int /*nShowCmd*/)
{
  init_apartment();

  Observable::observable_server<Configuration::LightConfiguration> configuration
  {
    make_unique<Networking::tcp_messaging_server>(9696ui16),
    make_unique<Threading::simple_dispatcher>()
  };

  MSG message;
  while (GetMessage(&message, NULL, 0, 0) > 0)
  {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }

  return 0;
}