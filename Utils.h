#pragma once

#include <memory>

void RemoveDisplaySleep();

template <class T> std::weak_ptr<T> make_weak_ptr(std::shared_ptr<T> ptr)
{
    return ptr;
}

