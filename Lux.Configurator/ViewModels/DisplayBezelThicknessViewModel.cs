using Lux.Configurator.Framework;
using System;
using System.ComponentModel;

namespace Lux.Configurator.ViewModels
{
  class DisplayBezelThicknessViewModel : INotifyPropertyChanged, INotifyObjectChanged
  {
    public event PropertyChangedEventHandler PropertyChanged;
    public event EventHandler ObjectChanged;

    private float _right;
    public float Right
    {
      get => _right;
      set
      {
        if (_right == value) return;

        _right = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Right)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }

    private float _top;
    public float Top
    {
      get => _top;
      set
      {
        if (_top == value) return;

        _top = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Top)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }

    private float _left;
    public float Left
    {
      get => _left;
      set
      {
        if (_left == value) return;

        _left = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Left)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }

    private float _bottom;
    public float Bottom
    {
      get => _bottom;
      set
      {
        if (_bottom == value) return;

        _bottom = value;
        PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Bottom)));
        ObjectChanged?.Invoke(this, EventArgs.Empty);
      }
    }
  }
}
