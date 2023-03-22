
#ifndef CAF_OPENSSL_EXPORT_H
#define CAF_OPENSSL_EXPORT_H

#ifdef LIBCAF_OPENSSL_STATIC_DEFINE
#  define CAF_OPENSSL_EXPORT
#  define LIBCAF_OPENSSL_NO_EXPORT
#else
#  ifndef CAF_OPENSSL_EXPORT
#    ifdef libcaf_openssl_EXPORTS
        /* We are building this library */
#      define CAF_OPENSSL_EXPORT __attribute__((visibility("default")))
#    else
        /* We are using this library */
#      define CAF_OPENSSL_EXPORT __attribute__((visibility("default")))
#    endif
#  endif

#  ifndef LIBCAF_OPENSSL_NO_EXPORT
#    define LIBCAF_OPENSSL_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef LIBCAF_OPENSSL_DEPRECATED
#  define LIBCAF_OPENSSL_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef LIBCAF_OPENSSL_DEPRECATED_EXPORT
#  define LIBCAF_OPENSSL_DEPRECATED_EXPORT CAF_OPENSSL_EXPORT LIBCAF_OPENSSL_DEPRECATED
#endif

#ifndef LIBCAF_OPENSSL_DEPRECATED_NO_EXPORT
#  define LIBCAF_OPENSSL_DEPRECATED_NO_EXPORT LIBCAF_OPENSSL_NO_EXPORT LIBCAF_OPENSSL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef LIBCAF_OPENSSL_NO_DEPRECATED
#    define LIBCAF_OPENSSL_NO_DEPRECATED
#  endif
#endif

#endif /* CAF_OPENSSL_EXPORT_H */
