using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace Lux.Configurator.Framework
{
  class ViewBase<TViewModel> : UserControl
    where TViewModel : class
  {
    public TViewModel ViewModel
    {
      get => GetValue(_viewModelProperty) as TViewModel;
      set => SetValue(_viewModelProperty, value);
    }

    private static DependencyProperty _viewModelProperty =
      DependencyProperty.Register(nameof(ViewModel), typeof(TViewModel), typeof(ViewBase<TViewModel>), new PropertyMetadata(null, OnViewModelChanged));

    private static void OnViewModelChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
      (d as ViewBase<TViewModel>).OnViewModelApplied(e.NewValue as TViewModel);
    }

    protected virtual void OnViewModelApplied(TViewModel viewModel)
    { }
  }
}
