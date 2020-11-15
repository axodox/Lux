#include "pch.h"
#include "IO.h"

using namespace std;
using namespace std::filesystem;
using namespace winrt;
using namespace winrt::Windows::Storage;

namespace Lux::IO
{
  path app_folder()
  {
    wstring filePath(MAX_PATH, L'\0');
    GetModuleFileName(nullptr, filePath.data(), (DWORD)filePath.size());
    return path(filePath).parent_path();
  }
  
  vector<uint8_t> load_file(const path& path)
  {
    auto file = StorageFile::GetFileFromPathAsync(path.c_str()).get();
    auto buffer = FileIO::ReadBufferAsync(file).get();
    
    vector<uint8_t> result(buffer.Length());
    memcpy(result.data(), buffer.data(), result.size());
    return result;
  }
}