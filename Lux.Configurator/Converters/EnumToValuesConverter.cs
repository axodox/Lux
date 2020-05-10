using System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace Lux.Configurator.Converters
{
  class EnumToValuesConverter : IValueConverter
  {
    public object Convert(object value, Type targetType, object parameter, string language)
    {
      return value?.GetType().IsEnum ?? false ? Enum.GetValues(value.GetType()) : DependencyProperty.UnsetValue;
    }

    public object ConvertBack(object value, Type targetType, object parameter, string language)
    {
      throw new NotSupportedException();
    }
  }
}
