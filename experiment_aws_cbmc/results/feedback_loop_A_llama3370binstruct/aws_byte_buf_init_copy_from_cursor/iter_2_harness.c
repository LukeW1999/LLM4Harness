#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor src;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);

    struct aws_byte_cursor old_src = src;
    struct aws_byte_buf old_dest = dest;

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        __CPROVER_assume(src.ptr != NULL);
        __CPROVER_assume(dest.buffer != NULL);
        __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
        __CPROVER_assume(dest.len <= MAX_BUFFER_SIZE);
        assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
    } else {
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
    }

    assert(dest.allocator == allocator);
    assert(aws_byte_buf_is_valid(&dest));
}
