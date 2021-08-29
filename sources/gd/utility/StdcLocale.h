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

#pragma once
#if defined(__APPLE__)
#   include <xlocale.h>
#endif
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

#include "StdcLocale.hpp"
