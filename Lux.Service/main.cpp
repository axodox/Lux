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

  observable_vector<observable_vector<int>> ls([](std::unique_ptr<change>&& change) {
    printf("%d", change->type());    
    });

  enum class my_prop_t : uint16_t
  {
    number
  };
  class my_object : public observable_object<my_prop_t>
  {
  public:
    observable_property<int> number;

    my_object(const observable::callback_t& callback) :
      observable_object<my_prop_t>(callback),
      number(make_property<int>(my_prop_t::number))
    { }
  } w([](std::unique_ptr<change>&& change) {
    printf("%d", change->type());
    });

  w.number.value(5);

  auto& x = ls.push_back();
  auto& y = x.push_back();
  auto z = y;

  memory_stream s;
  s.write(5);
  s.write(L"asd");

  MessageBox(0, L"Running.", 0, 0);
  return 0;
}