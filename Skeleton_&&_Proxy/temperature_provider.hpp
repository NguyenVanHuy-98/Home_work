#pragma once

#include <atomic>
#include <chrono>
#include <random>

#include "temperature_service.hpp"

class TemperatureProvider : public TemperatureSensorSkeleton {
public:
    explicit TemperatureProvider(ara::com::InstanceSpecifier instance);

    void Start();
    void Stop();

protected:
    float GetCurrentTemperature() override;

private:
    float ReadSensor();

    std::atomic<bool> running_{true};
    std::atomic<float> lastTemperature_{25.0f};
    std::chrono::steady_clock::time_point startTime_;
    std::mt19937 rng_{std::random_device{}()};
    std::normal_distribution<float> noise_{0.0f, 0.3f};
};
