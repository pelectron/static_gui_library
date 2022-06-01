#ifndef CONFIG_H
#define CONFIG_H
#include <cstddef>

#define ENABLE_CHARCONV 1
// #define ENABLE_CHARCONV 0

/// Uncomment one of the three following lines to set a default character type
// #define SGL_CHAR_TYPE char
// #define SGL_CHAR_TYPE char16_t
// #define SGL_CHAR_TYPE char32_t

/// Uncomment next line and replace XXX with a number to set default line width
//#define SGL_LINE_WIDTH XXX

#ifndef SGL_INSTANTIATE
  #define SGL_INSTANTIATE 0
#endif
#if SGL_INSTANTIATE == 1
  #ifndef SGL_CHAR_TYPE
    #error SGL_CHAR_TYPE must be defined if SGL_INSTANTIATE is set to 1.
  #endif
  #ifndef SGL_LINE_WIDTH
    #error SGL_LINE_WIDTH must be defined if SGL_INSTANTIATE is set to 1.
  #endif
#endif

#endif