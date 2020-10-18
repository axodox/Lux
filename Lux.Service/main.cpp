#include "pch.h"
#include "Events.h"
#include "Data.h"
#include "MemoryStream.h"
#include "Serializer.h"
//using namespace winrt;
//using namespace Windows::Foundation;

//int main()
//{
//    init_apartment();
//    Uri uri(L"http://aka.ms/cppwinrt");
//    printf("Hello, %ls!\n", uri.AbsoluteUri().c_str());
//}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
  //init_apartment();
  
  using namespace Lux::Events;
  using namespace Lux::Data;
  using namespace Lux::Streams;

  /*observable_value<int> x;
  auto sub = x.changed([](auto, const int& i) {
    printf("%d", i);
    });
  x.value(5);*/

  observable_list<int> ls([](std::unique_ptr<change>&&) {});

  memory_stream s;
  s.write(5);
  s.write(L"asd");

  MessageBox(0, L"Running.", 0, 0);
  return 0;
}