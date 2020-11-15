#pragma once
#include "LightSource.h"
#include "Rect.h"
#include "BackgroundThread.h"

namespace Lux::Sources
{
  class DesktopSource : public LightSource
  {
  public:
    DesktopSource() = default;
    virtual ~DesktopSource();

    virtual Configuration::LightSourceKind Kind() override;

  private:
    static const size_t _verticalDivisions = 16u;
    float _sampleSize = 0.1f;
    bool _isWorkerActive = false;
    std::unique_ptr<Threading::background_thread> _workerThread;

    std::pair<std::vector<Math::rect>, std::vector<std::vector<std::pair<uint16_t, float>>>> CalculateSamplingRegions(const Configuration::DisplaySettings& settings);

    virtual void OnSettingsChanged() override;

    void Capture();
  };
}