#pragma once
#include "DependencyContainer.h"

namespace Lux
{
  class DependencyConfiguration
  {
  private:
    Infrastructure::dependency_container _container;

    DependencyConfiguration();

  public:
    static Infrastructure::dependency_container& Instance();
  };
}