using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Lux.Configurator.Framework
{
  interface INotifyObjectChanged
  {
    event EventHandler ObjectChanged;
  }
}
