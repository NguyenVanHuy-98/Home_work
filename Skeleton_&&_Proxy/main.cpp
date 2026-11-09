#include <chrono>
#include <thread>

#include "log.hpp"
#include "temperature_consumer.hpp"
#include "temperature_provider.hpp"

int main() {
    using namespace std::chrono_literals;

    const ara::com::InstanceSpecifier instance = "/Vehicle/TemperatureSensor/1";
    Log("[Main]", "instance = ", instance);

    TemperatureConsumer consumer(instance);
    std::thread consumerThread([&consumer]() { consumer.Run(7s); });

    std::this_thread::sleep_for(1500ms);
    Log("[Main]", "starting provider...");

    TemperatureProvider provider(instance);
    std::thread providerThread([&provider]() { provider.Start(); });

    consumerThread.join();
    provider.Stop();
    providerThread.join();

    Log("[Main]", "demo finished");
    return 0;
}
