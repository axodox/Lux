using System;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Data;

namespace Lux.Configurator.Converters
{
  class EnumToDisplayNameConverter : IValueConverter
  {
    public object Convert(object value, Type targetType, object parameter, string language)
    {
      return value?.GetType().GetMember(value.ToString()).FirstOrDefault()?.GetCustomAttribute<DisplayNameAttribute>()?.DisplayName ?? DependencyProperty.UnsetValue;
    }

    public object ConvertBack(object value, Type targetType, object parameter, string language)
    {
      throw new NotImplementedException();
    }
  }
}
