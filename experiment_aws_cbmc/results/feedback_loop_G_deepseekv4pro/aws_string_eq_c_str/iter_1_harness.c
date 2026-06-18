#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 16

/* Reference implementation of the equality semantics */
static bool ref_eq(const struct aws_string *str, const char *c_str) {
    if (str == NULL && c_str == NULL) {
        return true;
    }
    if (str == NULL || c_str == NULL) {
        return false;
    }
    if (c_str[str->len] != 0) {
        return false;
    }
    for (size_t i = 0; i < str->len; i++) {
        if (str->bytes[i] != (uint8_t)c_str[i]) {
            return false;
        }
    }
    return true;
}

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    /* Non‑deterministically choose whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate enough space for the header + the bytes (including null terminator) */
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* Set the allocator (use default, non‑NULL) */
        str->allocator = aws_default_allocator();

        /* Write the length (cast away const – legal for malloc’d memory) */
        *((size_t *)&str->len) = len;

        /* Fill the data bytes non‑deterministically */
        for (size_t i = 0; i < len; i++) {
            str->bytes[i] = nondet_uint8_t();
        }
        /* Place a null terminator at position len (required by aws_string_is_valid) */
        str->bytes[len] = 0;

        /* Assume the constructed string passes the validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non‑deterministically choose whether c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        if (str != NULL) {
            /* When str is valid, allocate a buffer that is at least str->len+1 bytes,
             * so that the function can safely read c_str[str->len]. */
            size_t buf_size = str->len + 1;
            char *buf = (char *)malloc(buf_size);
            __CPROVER_assume(buf != NULL);

            for (size_t i = 0; i < str->len; i++) {
                buf[i] = nondet_uint8_t();
            }
            buf[str->len] = 0;  /* guarantee null terminator at str->len */
            c_str = buf;
        } else {
            /* str is NULL – allocate a bounded NUL‑terminated C string */
            size_t c_len = nondet_size_t();
            __CPROVER_assume(c_len <= MAX_STRING_LEN);
            char *buf = (char *)malloc(c_len + 1);
            __CPROVER_assume(buf != NULL);

            for (size_t i = 0; i < c_len; i++) {
                buf[i] = nondet_uint8_t();
            }
            buf[c_len] = 0;
            c_str = buf;
        }
    }

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postcondition: the returned value must match the reference implementation */
    assert(result == ref_eq(str, c_str));
}
