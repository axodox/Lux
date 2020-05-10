using Lux.Configurator.Framework;
using Lux.Configurator.ViewModels;

namespace Lux.Configurator.Views
{
  class DisplayBezelThicknessViewBase : ViewBase<DisplayBezelThicknessViewModel> { }

  sealed partial class DisplayBezelThicknessView : DisplayBezelThicknessViewBase
  {
    public DisplayBezelThicknessView()
    {
      InitializeComponent();
    }
  }
}
