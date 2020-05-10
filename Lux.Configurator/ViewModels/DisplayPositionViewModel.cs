using Lux.Configurator.Framework;
using System;
using System.ComponentModel;

namespace Lux.Configurator.ViewModels
{
  enum DisplayPositionReference
  {
    [DisplayName("┘")]
    BottomRight,
    [DisplayName("┐")]
    TopRight,
    [DisplayName("┌")]
    TopLeft,
    [DisplayName("└")]
    BottomLeft
  }

  class DisplayPositionViewModel : INotifyPropertyChanged, INotifyObjectChanged
  {
    public event EventHandler ObjectChanged;
    public event PropertyChangedEventHandler PropertyChanged;

    private DisplayPositionReference _positionReference;
    public DisplayPositionReference PositionReference
    {
      get => _positionReference;
      set
      {
        if (_positionReference == value) return;

        _positionReference = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(PositionReference)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }

    private float _x;
    public float X
    {
      get => _x;
      set
      {
        if (_x == value) return;

        _x = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(X)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }

    private float _y;
    public float Y
    {
      get => _y;
      set
      {
        if (_y == value) return;

        _y = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Y)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }
  }
}
