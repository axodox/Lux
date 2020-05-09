#pragma once
#include "App.xaml.g.h"

namespace winrt::Lux_Configurator::implementation
{
  struct App : AppT<App>
  {
    App();

    void OnLaunched(Windows::ApplicationModel::Activation::LaunchActivatedEventArgs const&);
    void OnSuspending(IInspectable const&, Windows::ApplicationModel::SuspendingEventArgs const&);
    void OnNavigationFailed(IInspectable const&, Windows::UI::Xaml::Navigation::NavigationFailedEventArgs const&);
    void OnBackgroundActivated(Windows::ApplicationModel::Activation::BackgroundActivatedEventArgs const& args);
    void OnTaskCanceled(Windows::ApplicationModel::Background::IBackgroundTaskInstance const& sender, Windows::ApplicationModel::Background::BackgroundTaskCancellationReason const& reason);

    inline static Windows::ApplicationModel::AppService::AppServiceConnection Connection = nullptr;

  private:
    Windows::ApplicationModel::Background::BackgroundTaskDeferral _appServiceDeferral = nullptr;
  };
}
