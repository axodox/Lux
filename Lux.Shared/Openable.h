#pragma once
#include "shared_pch.h"

namespace Lux::Infrastructure
{
  class openable
  {
  public:
    bool is_open() const;
    void open();

    virtual ~openable() = default;

  protected:
    virtual void on_opening() = 0;

  private:
    bool _is_open = false;
  };
}