#include "Locale.h"
#include <system_error>
#include <cerrno>

Locale Locale::create(int lc, const char *name)
{
#if defined(_WIN32)
    handle_type handle = _create_locale(lc, name);
#else
    handle_type handle;
    switch (lc) {
    case LC_ALL:
        handle = newlocale(LC_ALL_MASK, name, (locale_t)0);
        break;
    case LC_CTYPE:
        handle = newlocale(LC_CTYPE_MASK, name, (locale_t)0);
        break;
    case LC_COLLATE:
        handle = newlocale(LC_COLLATE_MASK, name, (locale_t)0);
        break;
    case LC_MONETARY:
        handle = newlocale(LC_MONETARY_MASK, name, (locale_t)0);
        break;
    case LC_NUMERIC:
        handle = newlocale(LC_NUMERIC_MASK, name, (locale_t)0);
        break;
    case LC_TIME:
        handle = newlocale(LC_TIME_MASK, name, (locale_t)0);
        break;
    case LC_MESSAGES:
        handle = newlocale(LC_MESSAGES_MASK, name, (locale_t)0);
        break;
    default:
        handle = (handle_type)0;
        errno = EINVAL;
        break;
    }
#endif
    if (handle == (handle_type)0)
        throw std::system_error(errno, std::generic_category());
    return Locale(handle);
}
