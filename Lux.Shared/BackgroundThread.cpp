#include "pch.h"
#include "BackgroundThread.h"
#include "ThreadName.h"

using namespace std;
using namespace std::chrono;
using namespace winrt;

namespace Lux::Threading
{
  background_thread::background_thread(const std::function<void()>& action, const std::wstring& name) :
    _name(name),
    _action(action),
    _worker(handle(CreateThread(nullptr, 0u, &background_thread::worker, this, 0u, nullptr)))
  { }

  DWORD WINAPI background_thread::worker(void* param)
  {
    background_thread* that = (background_thread*)param;

    set_thread_name(L"* " + that->_name);

    try
    {
      that->_action();
    }
    catch (...)
    {
      //log
    }

    return 0;
  }

  void background_thread::wait() const
  {
    if (is_current()) return;
    WaitForSingleObject(_worker.get(), INFINITE);
  }

  bool background_thread::wait_for(duration<uint32_t, milli> time) const
  {
    return WaitForSingleObject(_worker.get(), time.count()) == WAIT_OBJECT_0;
  }

  bool background_thread::is_running() const
  {
    return WaitForSingleObject(_worker.get(), 0u) != WAIT_OBJECT_0;
  }

  bool background_thread::is_current() const
  {
    return GetThreadId(GetCurrentThread()) == GetThreadId(_worker.get());
  }

  bool background_thread::is_shutting_down() const
  {
    return _is_shutting_down;
  }

  background_thread::~background_thread()
  {
    assert(!is_current());
    _is_shutting_down = true;
    wait();
  }
}