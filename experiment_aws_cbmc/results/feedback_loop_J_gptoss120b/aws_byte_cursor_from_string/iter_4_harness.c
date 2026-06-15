#include <aws/common/string.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness(void) {
    const struct aws_string *src;

    if (nondet_bool()) {
        src = NULL;
    } else {
        const size_t MAX_STRING_LEN = 256;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_LEN];
        } src_buf;

        src_buf.s.allocator = aws_default_allocator();
        src_buf.s.len = len;

        for (size_t i = 0; i < len; ++i) {
            src_buf.bytes[i] = nondet_uint8_t();
        }

        src = &src_buf.s;
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Preserve a copy of the original string contents for later comparison */
    struct aws_string old;
    uint8_t *old_bytes = NULL;
    if (src) {
        old.allocator = src->allocator;
        old.len = src->len;

        if (src->len > 0) {
            old_bytes = malloc(src->len);
            if (old_bytes) {
                for (size_t i = 0; i < src->len; ++i) {
                    old_bytes[i] = src->bytes[i];
                }
            }
        }
    }

    struct aws_byte_cursor cursor = aws_byte_cursor_from_string(src);

    if (src == NULL) {
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        assert(cursor.ptr == aws_string_bytes(src));
        assert(cursor.len == src->len);

        /* Ensure the original string was not modified */
        assert(src->allocator == old.allocator);
        assert(src->len == old.len);
        if (old_bytes) {
            for (size_t i = 0; i < src->len; ++i) {
                assert(src->bytes[i] == old_bytes[i]);
            }
        }

        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_bounded(&cursor, src->len));
    }
}
