#include "StopWatch.hpp"

namespace photon::util {
StopWatch::StopWatch() {
  m_initialTime = std::chrono::high_resolution_clock::now();

  m_lastTime = std::chrono::high_resolution_clock::now();
}

float StopWatch::TimeStep() {
  const auto currentTime = std::chrono::high_resolution_clock::now();

  auto timeDuration =
      std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_lastTime).count();

  m_lastTime = currentTime;

  return timeDuration;
}

float StopWatch::TimeStepSinceInitialisation() {
  auto currentTime = std::chrono::high_resolution_clock::now();

  auto time_duration =
      std::chrono::duration<float, std::chrono::seconds::period>(currentTime - m_initialTime)
          .count();

  return time_duration;
}
}  // namespace photon::util
