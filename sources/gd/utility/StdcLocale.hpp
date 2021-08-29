/* Copyright (c) 2021, Jean Pierre Cimalando
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

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
