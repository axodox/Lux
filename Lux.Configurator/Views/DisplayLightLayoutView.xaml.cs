using Lux.Configurator.Framework;
using Lux.Configurator.ViewModels;

namespace Lux.Configurator.Views
{
  class DisplayLightLayoutViewBase : ViewBase<DisplayLightLayoutViewModel> { }

  sealed partial class DisplayLightLayoutView : DisplayLightLayoutViewBase
  {
    public DisplayLightLayoutView()
    {
      ViewModel = new DisplayLightLayoutViewModel();
      InitializeComponent();
    }
  }
}
