#include "pch.h"
#include "LightService.h"

using namespace Lux::Service;
using namespace std;
using namespace winrt;

int WINAPI wWinMain(
  _In_ HINSTANCE /*hInstance*/,
  _In_opt_ HINSTANCE /*hPrevInstance*/,
  _In_ LPWSTR /*lpCmdLine*/,
  _In_ int /*nShowCmd*/)
{
  auto event = handle(CreateEvent(nullptr, false, true, L"Lux.Service.IsRunning"));
  if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;

  check_bool(SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2));

  init_apartment();
  LightService{}.Run();
  return 0;
}