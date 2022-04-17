
#ifndef REFL_API_H
#define REFL_API_H

#ifdef REFL_STATIC_DEFINE
#  define REFL_API
#  define REFL_NO_EXPORT
#else
#  ifndef REFL_API
#    ifdef Refl_EXPORTS
        /* We are building this library */
#      define REFL_API __declspec(dllexport)
#    else
        /* We are using this library */
#      define REFL_API __declspec(dllimport)
#    endif
#  endif

#  ifndef REFL_NO_EXPORT
#    define REFL_NO_EXPORT 
#  endif
#endif

#ifndef REFL_DEPRECATED
#  define REFL_DEPRECATED __declspec(deprecated)
#endif

#ifndef REFL_DEPRECATED_EXPORT
#  define REFL_DEPRECATED_EXPORT REFL_API REFL_DEPRECATED
#endif

#ifndef REFL_DEPRECATED_NO_EXPORT
#  define REFL_DEPRECATED_NO_EXPORT REFL_NO_EXPORT REFL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef REFL_NO_DEPRECATED
#    define REFL_NO_DEPRECATED
#  endif
#endif

#endif /* REFL_API_H */
