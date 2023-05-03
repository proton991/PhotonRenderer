#pragma once

#include <chrono>

namespace photon::util {
class StopWatch {
  // The time point of the last render call.
  std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;

  // The time point of initialisation.
  std::chrono::time_point<std::chrono::high_resolution_clock> m_initialTime;

public:
  StopWatch();

  /// @brief Return a scaling factor which corresponds to the
  /// time which has passed since last render call and now.
  [[nodiscard]] float TimeStep();

  /// @brief Return a scaling factor which corresponds to the
  /// time which has passed since initialisation and now.
  [[nodiscard]] float TimeStepSinceInitialisation();
};
}  // namespace photon::util
