#pragma once

//Std
#include <string>
#include <functional>

#include <queue>
#include <unordered_map>

#include <shared_mutex>
#include <future>

//Win32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 
#endif // !WIN32_LEAN_AND_MEAN

#include <unknwn.h>
#include <restrictederrorinfo.h>
#include <hstring.h>
#include <Windows.h>

#undef min
#undef max

//WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Networking.h>
#include <winrt/Windows.Networking.Sockets.h>