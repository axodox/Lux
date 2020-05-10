using Lux.Configurator.Framework;
using Lux.Configurator.ViewModels;
using Windows.Globalization.NumberFormatting;

namespace Lux.Configurator.Views
{
  class DisplayPositionViewBase : ViewBase<DisplayPositionViewModel> { }

  sealed partial class DisplayPositionView : DisplayPositionViewBase
  {
    public DisplayPositionView()
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
      _xBox.NumberFormatter = formatter;
      _yBox.NumberFormatter = formatter;
    }
  }
}
