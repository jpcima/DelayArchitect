#pragma once
#include <memory>
#include <list>

class AutoDeletePool {
public:
    void clear() noexcept;
    void add(void *ptr, void(*del)(void *));
    template <class T, class... A> T *makeNew(A &&... args);

private:
    using AutoPtr = std::unique_ptr<void, void(*)(void *)>;
    std::list<AutoPtr> list_;
};

#include "AutoDeletePool.hpp"
