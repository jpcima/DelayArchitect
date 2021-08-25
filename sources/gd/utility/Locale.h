#pragma once
#include <clocale>
#include <cstddef>

class Locale {
public:
#if !defined(_WIN32)
    typedef locale_t handle_type;
#else
    typedef _locale_t handle_type;
#endif

    explicit Locale(handle_type handle = (handle_type)0) noexcept;
    ~Locale() noexcept;
    Locale(Locale &&locale) noexcept;
    Locale &operator=(Locale &&locale) noexcept;
    explicit operator bool() const noexcept;
    handle_type operator*() const noexcept;

    void reset(handle_type handle = (handle_type)0) noexcept;
    static Locale create(int lc, const char *name);

    template <class... A> int sprintf(char *str, const char *format, A... args);
    template <class... A> int snprintf(char *str, size_t size, const char *format, A... args);
    template <class... A> int sscanf(const char *str, const char *format, A... args);

private:
    Locale(const Locale &locale) = delete;
    Locale &operator=(const Locale &locale) = delete;

private:
    handle_type handle_ = (handle_type)0;
};

#include "Locale.hpp"
