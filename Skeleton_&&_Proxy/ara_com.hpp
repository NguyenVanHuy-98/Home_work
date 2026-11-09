#pragma once

#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ara::com {

using InstanceSpecifier = std::string;

template <typename T>
class Event {
public:
    struct State {
        std::mutex mutex;
        std::deque<T> samples;
        std::size_t maxSamples{0};
        bool subscribed{false};
        std::function<void()> receiveHandler;
    };

    Event() : state_(std::make_shared<State>()) {}
    explicit Event(std::shared_ptr<State> state) : state_(std::move(state)) {}

    void Send(const T& value) {
        std::function<void()> handler;
        {
            std::lock_guard<std::mutex> lock(state_->mutex);
            if (!state_->subscribed) {
                return;
            }
            state_->samples.push_back(value);
            while (state_->samples.size() > state_->maxSamples) {
                state_->samples.pop_front();
            }
            handler = state_->receiveHandler;
        }
        if (handler) {
            handler();
        }
    }

    void Subscribe(std::size_t maxSamples) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        state_->maxSamples = maxSamples;
        state_->subscribed = true;
    }

    void SetReceiveHandler(std::function<void()> handler) {
        std::lock_guard<std::mutex> lock(state_->mutex);
        state_->receiveHandler = std::move(handler);
    }

    std::vector<T> GetNewSamples() {
        std::lock_guard<std::mutex> lock(state_->mutex);
        std::vector<T> samples(state_->samples.begin(), state_->samples.end());
        state_->samples.clear();
        return samples;
    }

    std::shared_ptr<State> state() const { return state_; }

private:
    std::shared_ptr<State> state_;
};

struct ServiceInstance {
    InstanceSpecifier instance;
    std::shared_ptr<void> payload;
};

class ServiceHandle {
public:
    explicit ServiceHandle(std::shared_ptr<ServiceInstance> instance)
        : instance_(std::move(instance)) {}

    const InstanceSpecifier& GetInstanceSpecifier() const {
        return instance_->instance;
    }

    std::shared_ptr<ServiceInstance> instance() const { return instance_; }

private:
    std::shared_ptr<ServiceInstance> instance_;
};

class Registry {
public:
    static Registry& instance() {
        static Registry registry;
        return registry;
    }

    void add(const InstanceSpecifier& id, std::shared_ptr<ServiceInstance> service) {
        std::lock_guard<std::mutex> lock(mutex_);
        services_[id] = std::move(service);
    }

    void remove(const InstanceSpecifier& id) {
        std::lock_guard<std::mutex> lock(mutex_);
        services_.erase(id);
    }

    std::vector<std::shared_ptr<ServiceInstance>> find(const InstanceSpecifier& id) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::shared_ptr<ServiceInstance>> result;
        auto it = services_.find(id);
        if (it != services_.end()) {
            result.push_back(it->second);
        }
        return result;
    }

private:
    Registry() = default;

    std::mutex mutex_;
    std::unordered_map<InstanceSpecifier, std::shared_ptr<ServiceInstance>> services_;
};

}
