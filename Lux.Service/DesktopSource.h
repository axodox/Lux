#pragma once
#include "LightSource.h"
#include "Rect.h"
#include "BackgroundThread.h"
#include "CallAggregator.h"

namespace Lux::Sources
{
  class DesktopSource : public LightSource
  {
  public:
    DesktopSource();

    virtual Configuration::LightSourceKind Kind() override;

    float TemporalAveraging() const;
    void TemporalAveraging(float value);

    float SampleSize() const;
    void SampleSize(float value);

  private:
    static const size_t _verticalDivisions = 16u;
    float _sampleSize = 0.1f;
    float _temporalAveraging = 0.2f;

    std::unique_ptr<Threading::background_thread> _workerThread;
    Threading::call_aggregator<> _restartAggregator;

    std::pair<std::vector<Math::rect>, std::vector<std::vector<std::pair<uint16_t, float>>>> CalculateSamplingRegions(const Configuration::DisplaySettings& settings);

    void RestartCapture();
    virtual void OnSettingsChanged() override;

    void Capture();
  };
}