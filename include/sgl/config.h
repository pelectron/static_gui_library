#ifndef SGL_CONFIG_H
#define SGL_CONFIG_H

/// Uncomment one of the three following lines to set a default character type
// #define SGL_CHAR_TYPE char
// #define SGL_CHAR_TYPE char16_t
// #define SGL_CHAR_TYPE char32_t

/// Uncomment next line and replace XXX with a number to set default line width
//#define SGL_LINE_WIDTH XXX

#ifndef SGL_INSTANTIATE
  #define SGL_INSTANTIATE 0
#endif
#ifndef SGL_CHAR_TYPE
  #define SGL_CHAR_TYPE char
#endif
#ifndef SGL_LINE_WIDTH
  #define SGL_LINE_WIDTH 40
#endif
#if SGL_INSTANTIATE && !defined(SGL_CHAR_TYPE) && !defined(SGL_LINE_WIDTH)
  #error SGL_CHAR_TYPE and SGL_LINE_WIDTH must be defined if SGL_INSTANTIATE is set to 1.
#endif

#if defined(_MSC_VER)
    //  Microsoft 
    #define SGL_EXPORT __declspec(dllexport)
    #define SGL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define SGL_EXPORT __attribute__((visibility("default")))
    #define SGL_IMPORT
#else
    //  do nothing and hope for the best?
    #define SGL_EXPORT
    #define SGL_IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif
#if defined(SGL_BUILD_LIB)
#   define SGL_API SGL_EXPORT
#else
#   define SGL_API SGL_IMPORT
#endif
#endif /* SGL_CONFIG_H */
