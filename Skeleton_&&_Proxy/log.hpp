#pragma once

#include <iostream>
#include <mutex>
#include <string>
#include <utility>

inline std::mutex& logMutex() {
    static std::mutex mutex;
    return mutex;
}

template <typename... Args>
void Log(const std::string& tag, Args&&... args) {
    std::lock_guard<std::mutex> lock(logMutex());
    std::cout << tag << ' ';
    (std::cout << ... << std::forward<Args>(args));
    std::cout << '\n';
}
