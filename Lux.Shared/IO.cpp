#include "pch.h"
#include "IO.h"

namespace Lux::IO
{
  std::filesystem::path app_folder()
  {
    std::wstring filePath(MAX_PATH, L'\0');
    GetModuleFileName(nullptr, filePath.data(), (DWORD)filePath.size());
    return std::filesystem::path(filePath).parent_path();
  }
}