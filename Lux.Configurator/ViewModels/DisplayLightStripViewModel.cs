using Lux.Configurator.Framework;
using System;
using System.ComponentModel;

namespace Lux.Configurator.ViewModels
{
  class DisplayLightStripViewModel : INotifyPropertyChanged, INotifyObjectChanged
  {
    public event EventHandler ObjectChanged;

    public event PropertyChangedEventHandler PropertyChanged;

    public DisplayPositionViewModel Position { get; } = new DisplayPositionViewModel();

    private uint _lightCount;
    public uint LightCount
    {
      get => _lightCount;
      set
      {
        if (_lightCount == value) return;

        _lightCount = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(LightCount)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }

    private bool _isEnabled = true;
    public bool IsEnabled
    {
      get => _isEnabled;
      set
      {
        if (_isEnabled == value) return;

        _isEnabled = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsEnabled)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }

    public DisplayLightStripViewModel()
    {
      Position.ObjectChanged += (o, e) => ObjectChanged?.Invoke(this, EventArgs.Empty);
    }
  }
}
