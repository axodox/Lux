#pragma once
#include "shared_pch.h"

namespace Lux::Threading
{
  class background_thread
  {
  public:
    background_thread(const background_thread&) = delete;
    const background_thread& operator =(const background_thread&) = delete;

    background_thread(background_thread&&) = delete;
    background_thread& operator =(background_thread&&) = delete;

    background_thread(const std::function<void()>& action, const std::wstring& name = L"background thread");
    ~background_thread();

    bool is_running() const;
    bool is_current() const;
    bool is_shutting_down() const;

    void wait() const;
    bool wait_for(std::chrono::duration<uint32_t, std::milli> time) const;

  private:
    std::wstring _name;
    std::function<void()> _action;
    winrt::handle _worker;
    bool _is_shutting_down = false;

    static DWORD WINAPI worker(void* param);
  };
}