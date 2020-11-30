#pragma once
#include "shared_pch.h"

namespace Lux::IO
{
  std::filesystem::path app_folder();

  std::vector<uint8_t> load_file(const std::filesystem::path& path);
}