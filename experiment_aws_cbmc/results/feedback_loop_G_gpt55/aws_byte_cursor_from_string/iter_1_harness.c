#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_from_string_harness() {
    if (nondet_bool()) {
        const struct aws_string *src = NULL;

        struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

        assert(result.len == 0);
        assert(result.ptr == NULL);
        assert(aws_byte_cursor_is_valid(&result));
    } else {
        uint8_t bytes[MAX_BUFFER_SIZE + 1];
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        struct aws_string *src = aws_string_new_from_array(aws_default_allocator(), bytes, len);
        __CPROVER_assume(src != NULL);
        __CPROVER_assume(aws_string_is_valid(src));

        struct aws_allocator *old_allocator = src->allocator;
        size_t old_len = src->len;
        const uint8_t *old_bytes = src->bytes;

        struct store_byte_from_buffer old_byte;
        save_byte_from_array(old_bytes, old_len + 1, &old_byte);

        struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

        assert(result.len == old_len);
        assert(result.ptr == old_bytes);

        assert(aws_string_is_valid(src));
        assert(src->allocator == old_allocator);
        assert(src->len == old_len);
        assert(src->bytes == old_bytes);
        assert_byte_from_buffer_matches(old_bytes, &old_byte);

        assert(aws_byte_cursor_is_valid(&result));
        assert_bytes_match(result.ptr, old_bytes, result.len);
    }
}
