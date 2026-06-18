#include <aws/common/common.h>
#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Maximum length for nondeterministic strings */
#define MAX_STRING_LEN 256

/* Helper to create a nondeterministic C string (null‑terminated) */
static char *make_nondet_c_str(void) {
    size_t len = (size_t)__CPROVER_nondet_uint();
    __CPROVER_assume(len < MAX_STRING_LEN);
    char *buf = (char *)malloc(len + 1);
    __CPROVER_assume(buf != NULL);
    for (size_t i = 0; i < len; ++i) {
        buf[i] = (char)__CPROVER_nondet_uint();
    }
    buf[len] = '\0';
    return buf;
}

/* Helper to compute length of a C string (excluding the terminating NUL) */
static size_t c_str_len(const char *s) {
    size_t i = 0;
    while (s[i] != '\0') {
        ++i;
    }
    return i;
}

/* Helper to copy a buffer */
static void copy_bytes(const uint8_t *src, uint8_t *dst, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        dst[i] = src[i];
    }
}

/* Helper to copy a C string */
static void copy_c_str(const char *src, char *dst, size_t len) {
    for (size_t i = 0; i <= len; ++i) {
        dst[i] = src[i];
    }
}

/* Harness */
int aws_string_eq_c_str_harness(void) {
    const struct aws_string *str = NULL;
    bool have_str = __CPROVER_nondet_bool();

    const char *c_str = NULL;
    bool have_c_str = __CPROVER_nondet_bool();

    char *src_for_string = make_nondet_c_str();

    if (have_str) {
        struct aws_string *tmp = aws_string_new_from_c_str(aws_default_allocator(), src_for_string);
        __CPROVER_assume(tmp != NULL);
        str = tmp;
    } else {
        str = NULL;
    }

    if (have_c_str) {
        c_str = make_nondet_c_str();
    } else {
        c_str = NULL;
    }

    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    size_t str_len_snapshot = 0;
    uint8_t *str_bytes_snapshot = NULL;
    if (str != NULL) {
        str_len_snapshot = str->len;
        str_bytes_snapshot = (uint8_t *)malloc(str_len_snapshot);
        __CPROVER_assume(str_bytes_snapshot != NULL);
        copy_bytes(str->bytes, str_bytes_snapshot, str_len_snapshot);
    }

    size_t c_str_len_snapshot = 0;
    char *c_str_snapshot = NULL;
    if (c_str != NULL) {
        c_str_len_snapshot = c_str_len(c_str);
        c_str_snapshot = (char *)malloc(c_str_len_snapshot + 1);
        __CPROVER_assume(c_str_snapshot != NULL);
        copy_c_str(c_str, c_str_snapshot, c_str_len_snapshot);
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = aws_array_eq_c_str(str->bytes, str->len, c_str);
        assert(result == expected);
    }

    if (str != NULL) {
        assert(str->len == str_len_snapshot);
        for (size_t i = 0; i < str_len_snapshot; ++i) {
            assert(str->bytes[i] == str_bytes_snapshot[i]);
        }
    }
    if (c_str != NULL) {
        for (size_t i = 0; i <= c_str_len_snapshot; ++i) {
            assert(c_str[i] == c_str_snapshot[i]);
        }
    }

    if (have_str && str != NULL) {
        aws_string_destroy((struct aws_string *)str);
    }
    if (have_c_str && c_str != NULL) {
        free((void *)c_str);
    }
    free(src_for_string);
    free(str_bytes_snapshot);
    free(c_str_snapshot);
    return 0;
}
