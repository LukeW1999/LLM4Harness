#include <stdbool.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256U
#define MAX_CSTR_LEN   256U

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str = 0;
    const char *c_str = 0;

    uint8_t str_buf[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
    char c_str_buf[MAX_CSTR_LEN + 1];

    /* decide whether to allocate a string */
    if (__CPROVER_nondet_uint()) {
        size_t len = __CPROVER_nondet_uint();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str = (struct aws_string *)str_buf;
        str->allocator = aws_default_allocator();
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = __CPROVER_nondet_uchar();
        }
        ((uint8_t *)str->bytes)[len] = 0; /* null-terminator */
    }

    size_t c_len = 0;
    /* decide whether to allocate a C string */
    if (__CPROVER_nondet_uint()) {
        c_len = __CPROVER_nondet_uint();
        __CPROVER_assume(c_len < MAX_CSTR_LEN);
        c_str = c_str_buf;
        for (size_t i = 0; i < c_len; ++i) {
            c_str_buf[i] = (char)__CPROVER_nondet_uchar();
        }
        c_str_buf[c_len] = '\0';
    }

    /* save pre-state for frame condition checks */
    size_t saved_len = 0;
    uint8_t saved_bytes[MAX_STRING_LEN];
    struct aws_allocator *saved_allocator = 0;
    if (str != 0) {
        saved_len = str->len;
        saved_allocator = str->allocator;
        for (size_t i = 0; i < saved_len; ++i) {
            saved_bytes[i] = ((uint8_t *)str->bytes)[i];
        }
    }

    size_t saved_c_len = 0;
    char saved_c_str[MAX_CSTR_LEN + 1];
    if (c_str != 0) {
        saved_c_len = c_len;
        for (size_t i = 0; i <= saved_c_len; ++i) {
            saved_c_str[i] = c_str[i];
        }
    }

    /* call the function under verification */
    bool result = aws_string_eq_c_str(str, c_str);

    /* post-condition: return-value correctness */
    if (str == 0 && c_str == 0) {
        __CPROVER_assert(result == true, "both null -> true");
    } else if (str == 0 || c_str == 0) {
        __CPROVER_assert(result == false, "one null -> false");
    } else {
        bool expected = true;
        if (str->len != saved_c_len) {
            expected = false;
        } else {
            for (size_t i = 0; i < str->len; ++i) {
                if (((uint8_t *)str->bytes)[i] != (uint8_t)c_str[i]) {
                    expected = false;
                    break;
                }
            }
        }
        __CPROVER_assert(result == expected, "string equality result");
    }

    /* frame condition: str must be unchanged */
    if (str != 0) {
        __CPROVER_assert(str->len == saved_len, "len unchanged");
        __CPROVER_assert(str->allocator == saved_allocator, "allocator unchanged");
        for (size_t i = 0; i < saved_len; ++i) {
            __CPROVER_assert(((uint8_t *)str->bytes)[i] == saved_bytes[i], "bytes unchanged");
        }
    }

    /* frame condition: c_str must be unchanged */
    if (c_str != 0) {
        for (size_t i = 0; i <= saved_c_len; ++i) {
            __CPROVER_assert(c_str[i] == saved_c_str[i], "c_str unchanged");
        }
    }
}
