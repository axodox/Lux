#include "pch.h"
#include "SourcesView.h"
#if __has_include("SourcesView.g.cpp")
#include "SourcesView.g.cpp"
#endif

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::UI::Xaml;
using namespace winrt::Windows::UI::Xaml::Data;

namespace winrt::Lux::implementation
{
  SourcesView::SourcesView()
  {
    InitializeComponent();

    _viewModel.PropertyChanged({ this, &SourcesView::OnViewModelPropertyChanged });
  }

  SourcesViewModel SourcesView::ViewModel()
  {
    return _viewModel;
  }

  bool SourcesView::IsSourceOff()
  {
    return _viewModel.Source() == SourceKind::Off;
  }

  void SourcesView::IsSourceOff(bool value)
  {
    if (value)
    {
      _viewModel.Source(SourceKind::Off);
    }
  }

  bool SourcesView::IsSourceStatic()
  {
    return _viewModel.Source() == SourceKind::Static;
  }

  void SourcesView::IsSourceStatic(bool value)
  {
    if (value)
    {
      _viewModel.Source(SourceKind::Static);
    }
  }

  bool SourcesView::IsSourceRainbow()
  {
    return _viewModel.Source() == SourceKind::Rainbow;
  }

  void SourcesView::IsSourceRainbow(bool value)
  {
    if (value)
    {
      _viewModel.Source(SourceKind::Rainbow);
    }
  }

  bool SourcesView::IsSourceDesktop()
  {
    return _viewModel.Source() == SourceKind::Desktop;
  }

  void SourcesView::IsSourceDesktop(bool value)
  {
    if (value)
    {
      _viewModel.Source(SourceKind::Desktop);
    }
  }

  event_token SourcesView::PropertyChanged(PropertyChangedEventHandler const& value)
  {
    return _propertyChanged.add(value);
  }

  void SourcesView::PropertyChanged(event_token const& token)
  {
    _propertyChanged.remove(token);
  }

  void SourcesView::OnViewModelPropertyChanged(IInspectable const& /*sender*/, PropertyChangedEventArgs const& eventArgs)
  {
    if (eventArgs.PropertyName() == L"Source")
    {
      _propertyChanged(*this, PropertyChangedEventArgs(L"IsSourceOff"));
      _propertyChanged(*this, PropertyChangedEventArgs(L"IsSourceStatic"));
      _propertyChanged(*this, PropertyChangedEventArgs(L"IsSourceRainbow"));
      _propertyChanged(*this, PropertyChangedEventArgs(L"IsSourceDesktop"));
    }

    if (eventArgs.PropertyName() == L"")
    {
      _propertyChanged(*this, PropertyChangedEventArgs(L""));
    }
  }
}
