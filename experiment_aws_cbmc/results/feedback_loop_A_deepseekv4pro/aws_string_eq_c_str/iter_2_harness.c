#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

void aws_string_eq_c_str_harness() {
    /* non-deterministically choose whether str and c_str are NULL */
    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    const struct aws_string *str;
    const char *c_str;

    size_t str_len = 0;   /* only valid if str != NULL */
    size_t c_str_len = 0; /* only valid if c_str != NULL */

    if (str_is_null) {
        str = NULL;
    } else {
        /* allocate an aws_string with length up to MAX_STRING_LEN */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str_len = len;

        /* total size = header + extra bytes for the flexible array member */
        size_t total_size = sizeof(struct aws_string) + len;
        uint8_t *raw = (uint8_t *)malloc(total_size);
        __CPROVER_assume(raw != NULL);

        /* set allocator and len fields (bytes are filled below) */
        struct aws_string header = { .allocator = aws_default_allocator(), .len = len };
        memcpy(raw, &header, sizeof(header));

        /* fill the bytes array with non-deterministic values */
        for (size_t i = 0; i < len; ++i) {
            raw[offsetof(struct aws_string, bytes) + i] = nondet_uint8_t();
        }
        str = (const struct aws_string *)raw;

        /* only consider strings that satisfy the library's validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (c_str_is_null) {
        c_str = NULL;
    } else {
        /* allocate a C string with length up to MAX_STRING_LEN */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        c_str_len = len;

        char *buf = (char *)malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            /* ensure no embedded null before the terminator */
            buf[i] = (char)nondet_uint8_t();
            __CPROVER_assume(buf[i] != '\0');
        }
        buf[len] = '\0';
        c_str = buf;
    }

    /* call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* compute the expected result */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = false;
        if (str->len == c_str_len) {
            bool match = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (aws_string_bytes(str)[i] != (uint8_t)c_str[i]) {
                    match = false;
                    break;
                }
            }
            expected = match;
        }
    }

    /* postcondition: the returned bool matches the expected comparison */
    assert(result == expected);
}
