#pragma once

#include <chrono>
#include <optional>

#include "temperature_service.hpp"

class TemperatureConsumer {
public:
    explicit TemperatureConsumer(ara::com::InstanceSpecifier instance);

    bool WaitForService(std::chrono::milliseconds timeout);
    void Run(std::chrono::seconds duration);

private:
    ara::com::InstanceSpecifier instance_;
    std::optional<TemperatureSensorProxy> proxy_;
};
