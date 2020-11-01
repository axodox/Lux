#pragma once

//Warnings
#pragma warning ( disable : 4505 ) //unreferenced local function has been removed - caused by many WinRT headers

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

#undef GetObjectW
#undef min
#undef max

//WinRT
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Networking.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.Data.Json.h>