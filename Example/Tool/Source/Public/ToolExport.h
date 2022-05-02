
#ifndef TOOL_API_H
#define TOOL_API_H

#ifdef TOOL_STATIC_DEFINE
#  define TOOL_API
#  define TOOL_NO_EXPORT
#else
#  ifndef TOOL_API
#    ifdef Tool_EXPORTS
        /* We are building this library */
#      define TOOL_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define TOOL_API __declspec(dllimport)
#    endif
#  endif

#  ifndef TOOL_NO_EXPORT
#    define TOOL_NO_EXPORT 
#  endif
#endif

#ifndef TOOL_DEPRECATED
#  define TOOL_DEPRECATED __declspec(deprecated)
#endif

#ifndef TOOL_DEPRECATED_EXPORT
#  define TOOL_DEPRECATED_EXPORT TOOL_API TOOL_DEPRECATED
#endif

#ifndef TOOL_DEPRECATED_NO_EXPORT
#  define TOOL_DEPRECATED_NO_EXPORT TOOL_NO_EXPORT TOOL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef TOOL_NO_DEPRECATED
#    define TOOL_NO_DEPRECATED
#  endif
#endif

#endif /* TOOL_API_H */
