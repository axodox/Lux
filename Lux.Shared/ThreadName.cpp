#include "pch.h"
#include "ThreadName.h"

namespace Lux::Threading
{
  void set_thread_name(const std::wstring& name)
  {
    winrt::check_hresult(SetThreadDescription(GetCurrentThread(), name.c_str()));
  }

  std::wstring get_thread_name()
  {
    wchar_t* name;
    winrt::check_hresult(GetThreadDescription(GetCurrentThread(), &name));
    auto result = std::wstring(name);
    LocalFree(name);
    return result;
  }

  thread_name_context::thread_name_context(const std::wstring& name)
  {
    _name = get_thread_name();
    set_thread_name(name);
  }

  thread_name_context::~thread_name_context()
  {
    set_thread_name(_name);
  }
}