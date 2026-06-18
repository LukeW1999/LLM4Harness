#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* stack‑allocated contiguous memory for the string struct and its bytes */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } raw;

        str = &raw.s;
        str->allocator = NULL;
        str->len = len;

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    }

    struct aws_string old;
    uint8_t old_buf[MAX_STRING_LEN];
    uint8_t *old_bytes = NULL;
    if (str) {
        old = *str;
        if (str->len > 0) {
            old_bytes = old_buf;
            for (size_t i = 0; i < str->len; ++i) {
                old_bytes[i] = ((uint8_t *)aws_string_bytes(str))[i];
            }
        }
    }

    aws_string_destroy_secure(str);

    if (str) {
        assert(str->len == old.len);
        assert(str->allocator == old.allocator);

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < str->len; ++i) {
            assert(bytes[i] == 0);
        }

        if (str->allocator == NULL) {
            assert(aws_string_is_valid(str));
        }
    }
}
