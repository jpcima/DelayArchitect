#include "AutoDeletePool.h"

inline void AutoDeletePool::clear() noexcept
{
    list_.clear();
}

inline void AutoDeletePool::add(void *ptr, void(*del)(void *))
{
    list_.emplace_back(ptr, del);
}

template <class T, class... A>
T *AutoDeletePool::makeNew(A &&... args)
{
    T *obj = new T(std::forward<A>(args)...);
    void (*del)(void *) = +[](void *x) { delete reinterpret_cast<T *>(x); };
    add(obj, del);
    return obj;
}
