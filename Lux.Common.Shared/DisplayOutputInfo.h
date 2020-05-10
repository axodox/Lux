#pragma once
#include "DisplayOutputInfo.g.h"

namespace winrt::Lux::Common::implementation
{
  struct DisplayOutputInfo : DisplayOutputInfoT<DisplayOutputInfo>
  {
    DisplayOutputInfo() = default;

    static Windows::Foundation::Collections::IVector<Lux::Common::DisplayOutputInfo> GetOutputs();
    uint64_t AdapterId();
    void AdapterId(uint64_t value);
    hstring OutputId();
    void OutputId(hstring const& value);
    hstring Name();
    void Name(hstring const& value);

  private:
    uint64_t _adapterId;
    hstring _outputId;
    hstring _name;
  };
}
namespace winrt::Lux::Common::factory_implementation
{
  struct DisplayOutputInfo : DisplayOutputInfoT<DisplayOutputInfo, implementation::DisplayOutputInfo>
  {
  };
}
