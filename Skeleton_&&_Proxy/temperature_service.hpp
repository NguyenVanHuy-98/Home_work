#pragma once

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include "ara_com.hpp"

struct TemperatureSensorService {
    struct Payload {
        std::shared_ptr<ara::com::Event<float>::State> temperatureUpdate;
        std::function<float()> getCurrentTemperature;
    };
};

class TemperatureSensorSkeleton {
public:
    explicit TemperatureSensorSkeleton(ara::com::InstanceSpecifier instance)
        : TemperatureUpdate(std::make_shared<ara::com::Event<float>::State>()),
          instance_(std::move(instance)) {
        auto payload = std::make_shared<TemperatureSensorService::Payload>();
        payload->temperatureUpdate = TemperatureUpdate.state();
        payload->getCurrentTemperature = [this]() { return GetCurrentTemperature(); };

        service_ = std::make_shared<ara::com::ServiceInstance>();
        service_->instance = instance_;
        service_->payload = payload;

        ara::com::Registry::instance().add(instance_, service_);
    }

    virtual ~TemperatureSensorSkeleton() {
        ara::com::Registry::instance().remove(instance_);
    }

    ara::com::Event<float> TemperatureUpdate;

protected:
    virtual float GetCurrentTemperature() = 0;

private:
    ara::com::InstanceSpecifier instance_;
    std::shared_ptr<ara::com::ServiceInstance> service_;
};

class TemperatureSensorProxy {
public:
    explicit TemperatureSensorProxy(ara::com::ServiceHandle handle)
        : handle_(std::move(handle)) {
        auto payload = std::static_pointer_cast<TemperatureSensorService::Payload>(
            handle_.instance()->payload);

        TemperatureUpdate = ara::com::Event<float>(payload->temperatureUpdate);
        getCurrentTemperature_ = payload->getCurrentTemperature;
    }

    static std::vector<ara::com::ServiceHandle> FindService(
        const ara::com::InstanceSpecifier& instance) {
        std::vector<ara::com::ServiceHandle> handles;
        for (const auto& service : ara::com::Registry::instance().find(instance)) {
            handles.emplace_back(service);
        }
        return handles;
    }

    float GetCurrentTemperature() {
        return getCurrentTemperature_();
    }

    ara::com::Event<float> TemperatureUpdate;

private:
    ara::com::ServiceHandle handle_;
    std::function<float()> getCurrentTemperature_;
};
