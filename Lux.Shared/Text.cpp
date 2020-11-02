#include "pch.h"
#include "Text.h"

namespace Lux::Text
{
  class string_splitter
  {
  private:
    const std::wstring_view _text;
    typename wchar_t _separator;
    size_t _position;

  public:
    string_splitter(const std::wstring_view& text, typename wchar_t separator) :
      _text(text),
      _separator(separator),
      _position(0)
    {}

    void reset()
    {
      _position = 0;
    }

    bool finished() const
    {
      return _position == std::wstring::npos;
    }

    std::wstring_view next()
    {
      auto lastPosition = _position;
      _position = _text.find_first_of(_separator, _position);
      auto substrLength = finished() ? _text.length() - lastPosition : _position++ - lastPosition;

      return std::wstring_view(_text.data() + lastPosition, substrLength);
    }
  };

  void trim_left(std::wstring& str)
  {
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) {
      return !std::isspace(ch);
      }));
  }

  void trim_right(std::wstring& str)
  {
    str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) {
      return !std::isspace(ch);
      }).base(), str.end());
  }

  void trim(std::wstring& str)
  {
    trim_left(str);
    trim_right(str);
  }

  std::vector<std::wstring> split(const std::wstring_view& text, wchar_t delimiter)
  {
    std::vector<std::wstring> results;

    string_splitter splitter(text, delimiter);
    do
    {
      results.push_back(std::wstring{ splitter.next() });
    } 
    while (!splitter.finished());
    
    return results;
  }
}