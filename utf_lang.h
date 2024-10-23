#ifndef utf_lang_h_
#define utf_lang_h_

#if (defined __GNUC__ || defined __clang__) && defined __has_attribute
#    if __has_attribute(always_inline)
#        define UTF_INLINE __attribute__((always_inline)) inline
#    endif
#elif defined _MSC_VER
#    define UTF_INLINE __forceinline
#endif

#ifndef UTF_INLINE
#    if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#        define UTF_INLINE inline
#    endif
#endif

#endif /* !defined(utf_lang_h_) */
