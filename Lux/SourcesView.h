#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "SourcesView.g.h"

namespace winrt::Lux::implementation
{
  struct SourcesView : SourcesViewT<SourcesView>
  {
    SourcesView();

    SourcesViewModel ViewModel();

    bool IsSourceOff();
    void IsSourceOff(bool value);

    bool IsSourceStatic();
    void IsSourceStatic(bool value);

    bool IsSourceRainbow();
    void IsSourceRainbow(bool value);

    bool IsSourceDesktop();
    void IsSourceDesktop(bool value);

    winrt::event_token PropertyChanged(Windows::UI::Xaml::Data::PropertyChangedEventHandler const& handler);
    void PropertyChanged(winrt::event_token const& token);

  private:
    winrt::event<Windows::UI::Xaml::Data::PropertyChangedEventHandler> _propertyChanged;
    SourcesViewModel _viewModel{};

    void OnViewModelPropertyChanged(Windows::Foundation::IInspectable const& sender, Windows::UI::Xaml::Data::PropertyChangedEventArgs const& eventArgs);
  };
}

namespace winrt::Lux::factory_implementation
{
  struct SourcesView : SourcesViewT<SourcesView, implementation::SourcesView>
  {
  };
}
