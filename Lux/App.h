#pragma once
#include "App.xaml.g.h"

namespace winrt::Lux::implementation
{
  struct App : AppT<App>
  {
    App();

    void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
    void OnActivated(Windows::ApplicationModel::Activation::IActivatedEventArgs const&);
    void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
    void OnNavigationFailed(IInspectable const&, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const&);

  private:
    Microsoft::Gaming::XboxGameBar::XboxGameBarWidget _gamebarWidget{ nullptr };
    event_token _gamebarWidgetWindowClosedHandlerToken{};

    void OnWindowClosed(IInspectable const&, IInspectable const&);
    
    winrt::fire_and_forget EnsureService();
  };
}
