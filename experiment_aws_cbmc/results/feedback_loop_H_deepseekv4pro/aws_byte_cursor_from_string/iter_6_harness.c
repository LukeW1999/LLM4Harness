#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_cursor_from_string_harness() {
    uint8_t buf[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    struct aws_string *src = NULL;

    if (nondet_bool()) {
        src = NULL;
    } else {
        src = (struct aws_string *)buf;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* allocator may be NULL for static strings */
        src->allocator = NULL;
        src->len = len;

        /* fill string data with arbitrary bytes, then null-terminate */
        for (size_t i = 0; i < len; i++) {
            src->bytes[i] = nondet_uint8_t();
        }
        src->bytes[len] = '\0';

        __CPROVER_assume(aws_string_is_valid(src));
    }

    size_t old_len = (src != NULL) ? src->len : 0;

    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    assert(aws_byte_cursor_is_valid(&result));

    if (src == NULL) {
        assert(result.len == 0);
        assert(result.ptr == NULL);
    } else {
        assert(result.len == src->len);
        assert(result.ptr == aws_string_bytes(src));

        assert(aws_string_is_valid(src));
        assert(src->len == old_len);
    }
}
