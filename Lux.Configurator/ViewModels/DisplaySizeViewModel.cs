using Lux.Configurator.Framework;
using System;
using System.ComponentModel;

namespace Lux.Configurator.ViewModels
{
  class DisplaySizeViewModel : INotifyPropertyChanged, INotifyObjectChanged
  {
    public event EventHandler ObjectChanged;
    public event PropertyChangedEventHandler PropertyChanged;

    private float _width;
    public float Width
    {
      get => _width;
      set
      {
        if (_width == value) return;

        _width = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Width)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }

    private float _height;
    public float Height
    {
      get => _height;
      set
      {
        if (_height == value) return;

        _height = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Height)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }
  }
}
