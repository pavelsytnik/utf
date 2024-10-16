#ifndef utf_error_h_
#define utf_error_h_

typedef enum utf_error utf_error;

enum utf_error {
    UTF_OK = 0,
    UTF_INVALID_LEAD,
    UTF_INVALID_TRAIL,
    UTF_TRUNCATED,
    UTF_OVERLONG_SEQUENCE,
    UTF_INVALID_CODE_POINT
};

#endif /* !defined(utf_error_h_) */
