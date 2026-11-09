#include "temperature_provider.hpp"

#include <cmath>
#include <thread>

#include "log.hpp"

TemperatureProvider::TemperatureProvider(ara::com::InstanceSpecifier instance)
    : TemperatureSensorSkeleton(std::move(instance)),
      startTime_(std::chrono::steady_clock::now()) {}

void TemperatureProvider::Start() {
    Log("[Provider]", "skeleton registered, publishing every 1s");
    while (running_) {
        const float temperature = ReadSensor();
        lastTemperature_.store(temperature);
        TemperatureUpdate.Send(temperature);
        Log("[Provider]", "Send(TemperatureUpdate = ", temperature, " C)");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Log("[Provider]", "stopped");
}

void TemperatureProvider::Stop() {
    running_ = false;
}

float TemperatureProvider::GetCurrentTemperature() {
    return lastTemperature_.load();
}

float TemperatureProvider::ReadSensor() {
    const double seconds = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - startTime_).count();
    return 25.0f + 3.0f * std::sin(seconds) + noise_(rng_);
}
