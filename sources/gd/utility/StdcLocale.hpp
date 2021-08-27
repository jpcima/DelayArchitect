#include "StdcLocale.h"
#include <cstdio>

inline Locale::Locale(handle_type handle) noexcept
    : handle_(handle)
{
}

inline Locale::~Locale() noexcept
{
    reset();
}

inline Locale::Locale(Locale &&locale) noexcept
    : handle_(locale.handle_)
{
    locale.handle_ = (handle_type)0;
}

inline Locale &Locale::operator=(Locale &&locale) noexcept
{
    if (this != &locale) {
        reset(locale.handle_);
        locale.handle_ = (handle_type)0;
    }
    return *this;
}

inline Locale::operator bool() const noexcept
{
    return handle_ != (handle_type)0;
}

inline Locale::handle_type Locale::operator*() const noexcept
{
    return handle_;
}

inline void Locale::reset(handle_type handle) noexcept
{
    if (handle_ == handle)
        return;
    if (handle_) {
#if !defined(_WIN32)
        freelocale(handle_);
#else
        _free_locale(handle_);
#endif
    }
    handle_ = handle;
}

template <class... A> inline int Locale::sprintf(char *str, const char *format, A... args)
{
#if defined(_WIN32)
    return _sprintf_l(str, format, handle_, args...);
#else
    handle_type old = (handle_type)0;
    if (*this)
        old = uselocale(handle_);
    int ret = ::sprintf(str, format, args...);
    if (*this)
        uselocale(old);
    return ret;
#endif
}

template <class... A> inline int Locale::snprintf(char *str, size_t size, const char *format, A... args)
{
#if defined(_WIN32)
    return _snprintf_l(str, size, format, handle_, args...);
#else
    handle_type old = (handle_type)0;
    if (*this)
        old = uselocale(handle_);
    int ret = ::snprintf(str, size, format, args...);
    if (*this)
        uselocale(old);
    return ret;
#endif
}

template <class... A> inline int Locale::sscanf(const char *str, const char *format, A... args)
{
#if defined(_WIN32)
    return _sscanf_l(str, format, handle_, args...);
#else
    handle_type old = (handle_type)0;
    if (*this)
        old = uselocale(handle_);
    int ret = ::sscanf(str, format, args...);
    if (*this)
        uselocale(old);
    return ret;
#endif
}
