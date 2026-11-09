#include "temperature_consumer.hpp"

#include <thread>

#include "log.hpp"

TemperatureConsumer::TemperatureConsumer(ara::com::InstanceSpecifier instance)
    : instance_(std::move(instance)) {}

bool TemperatureConsumer::WaitForService(std::chrono::milliseconds timeout) {
    const auto deadline = std::chrono::steady_clock::now() + timeout;

    while (std::chrono::steady_clock::now() < deadline) {
        auto handles = TemperatureSensorProxy::FindService(instance_);
        if (!handles.empty()) {
            proxy_.emplace(handles.front());
            Log("[Consumer]", "FindService -> found service ", instance_);
            return true;
        }
        Log("[Consumer]", "FindService -> service not available, retry...");
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    return false;
}

void TemperatureConsumer::Run(std::chrono::seconds duration) {
    if (!WaitForService(std::chrono::milliseconds(5000))) {
        Log("[Consumer]", "service not found, give up");
        return;
    }

    proxy_->TemperatureUpdate.Subscribe(3);
    proxy_->TemperatureUpdate.SetReceiveHandler([this]() {
        for (float temperature : proxy_->TemperatureUpdate.GetNewSamples()) {
            Log("[Consumer]", "event TemperatureUpdate -> ", temperature, " C");
        }
    });
    Log("[Consumer]", "subscribed TemperatureUpdate");

    const auto end = std::chrono::steady_clock::now() + duration;
    auto nextMethodCall = std::chrono::steady_clock::now();

    while (std::chrono::steady_clock::now() < end) {
        if (std::chrono::steady_clock::now() >= nextMethodCall) {
            const float temperature = proxy_->GetCurrentTemperature();
            Log("[Consumer]", "GetCurrentTemperature() -> ", temperature, " C");
            nextMethodCall += std::chrono::seconds(3);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    Log("[Consumer]", "done");
}
