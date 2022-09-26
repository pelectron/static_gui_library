#ifndef SGL_CONFIG_H
#define SGL_CONFIG_H

// #include <cstddef>

#ifndef SGL_USE_RYU
  #define SGL_USE_RYU 0
#endif
#ifndef SGL_USE_CHARCONV
  #if SGL_USE_RYU
    #define SGL_USE_CHARCONV 0
  #else
    #define SGL_USE_CHARCONV 1
  #endif
#endif

#if SGL_USE_CHARCONV && SGL_USE_RYU
  #error SGL_USE_CHARCONV and SGL_USE_RYU cannot both be defined.
#endif

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

#endif /* SGL_CONFIG_H */
