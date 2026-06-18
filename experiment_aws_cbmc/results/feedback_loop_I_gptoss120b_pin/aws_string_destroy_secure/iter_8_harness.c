#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_allocator *default_allocator = aws_default_allocator();

    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Stack‑allocated memory for the string struct and its bytes */
        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } raw;

        str = &raw.s;
        str->len = len;

        /* Choose allocator: either NULL or the default allocator */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            str->allocator = default_allocator;
        }

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    size_t old_len = 0;
    struct aws_allocator *old_allocator = NULL;
    uint8_t old_buf[MAX_STRING_LEN];
    uint8_t *old_bytes = NULL;

    if (str) {
        old_len = str->len;
        old_allocator = str->allocator;
        if (str->len > 0) {
            old_bytes = old_buf;
            for (size_t i = 0; i < str->len; ++i) {
                old_bytes[i] = ((uint8_t *)aws_string_bytes(str))[i];
            }
        }
    }

    aws_string_destroy_secure(str);

    if (str) {
        assert(str->len == old_len);
        assert(str->allocator == old_allocator);

        uint8_t *bytes = (uint8_t *)aws_string_bytes(str);
        for (size_t i = 0; i < str->len; ++i) {
            assert(bytes[i] == 0);
        }

        if (str->allocator == NULL) {
            /* When allocator is NULL the string remains a valid object */
            assert(aws_string_is_valid(str));
        }
    }
}
