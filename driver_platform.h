#ifndef DRIVER_PLATFORM_H
#define DRIVER_PLATFORM_H

#if (defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__)
#define WARN_UNUSED_RESULT __attribute__ ((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1700)
#define WARN_UNUSED_RESULT _Check_return_
#else
#define WARN_UNUSED_RESULT
#endif

#endif
