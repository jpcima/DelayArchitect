#pragma once

template <class T>
T clamp(T x, T lo, T hi)
{
    x = (x > lo) ? x : lo;
    x = (x < hi) ? x : hi;
    return x;
}
