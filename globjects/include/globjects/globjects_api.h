
#ifndef GLOBJECTS_TEMPLATE_API_H
#define GLOBJECTS_TEMPLATE_API_H

#include <globjects/globjects_export.h>

#ifdef GLOBJECTS_STATIC_DEFINE
#  define GLOBJECTS_TEMPLATE_API
#else
#  ifndef GLOBJECTS_TEMPLATE_API
#    ifdef GLOBJECTS_EXPORTS
        /* We are building this library */
#      define GLOBJECTS_TEMPLATE_API __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define GLOBJECTS_TEMPLATE_API __attribute__((visibility("default")))
#    endif
#  endif

#endif

#endif
