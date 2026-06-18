#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_cursor old_src = src;

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_src.len);
        assert(dest.capacity == old_src.len);
        assert(dest.allocator == allocator);
        if (old_src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, old_src.ptr, old_src.len);
        } else {
            assert(dest.buffer == NULL);
        }
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(result == AWS_OP_ERR);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }

    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);
}
