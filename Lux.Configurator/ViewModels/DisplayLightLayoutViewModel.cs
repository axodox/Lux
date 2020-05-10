using Lux.Configurator.Framework;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.Linq;

namespace Lux.Configurator.ViewModels
{
  class DisplayLightLayoutViewModel : INotifyObjectChanged
  {
    public DisplaySizeViewModel DisplaySize { get; } = new DisplaySizeViewModel();

    public DisplayBezelThicknessViewModel BezelThickness { get; } = new DisplayBezelThicknessViewModel();

    public DisplayPositionViewModel StartPosition { get; } = new DisplayPositionViewModel();

    public ObservableCollection<DisplayLightStripViewModel> Segments { get; } = new ObservableCollection<DisplayLightStripViewModel>();

    public event EventHandler ObjectChanged;

    public DisplayLightLayoutViewModel()
    {
      DisplaySize.ObjectChanged += OnChildChanged;
      BezelThickness.ObjectChanged += OnChildChanged;
      StartPosition.ObjectChanged += OnChildChanged;
      Segments.CollectionChanged += OnSegmentsChanged;
    }

    private void OnSegmentsChanged(object sender, NotifyCollectionChangedEventArgs e)
    {
      if (e.OldItems != null)
      {
        foreach (DisplayLightStripViewModel segment in e.OldItems)
        {
          segment.ObjectChanged -= OnChildChanged;
        }
      }

      if (e.NewItems != null)
      {
        foreach (DisplayLightStripViewModel segment in e.NewItems)
        {
          segment.ObjectChanged += OnChildChanged;
        }
      }
    }

    private void OnChildChanged(object sender, EventArgs e)
    {
      ObjectChanged?.Invoke(this, EventArgs.Empty);
    }

    public void AddSegment()
    {
      Segments.Add(new DisplayLightStripViewModel());
    }

    public void RemoveSegment()
    {
      if(Segments.Any()) Segments.Remove(Segments.Last());
    }
  }
}
