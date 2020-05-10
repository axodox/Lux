using Lux.Configurator.Framework;
using Lux.Configurator.ViewModels;
using Windows.Globalization.NumberFormatting;

namespace Lux.Configurator.Views
{
  class DisplaySizeViewBase : ViewBase<DisplaySizeViewModel> { }

  sealed partial class DisplaySizeView : DisplaySizeViewBase
  {
    public DisplaySizeView()
    {
      InitializeComponent();

      var rounder = new IncrementNumberRounder()
      {
        Increment = 0.1,
        RoundingAlgorithm = RoundingAlgorithm.RoundHalfAwayFromZero
      };

      var formatter = new DecimalFormatter()
      {
        FractionDigits = 1,
        NumberRounder = rounder
      };
      _widthBox.NumberFormatter = formatter;
      _heightBox.NumberFormatter = formatter;
    }
  }
}
