#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_buf_from_array_harness(void) {
    struct aws_byte_cursor src;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer old_src_byte;
    if (old_src.len > 0) {
        save_byte_from_array(old_src.ptr, old_src.len, &old_src_byte);
    }

    struct aws_byte_buf buf = aws_byte_buf_from_array(src.ptr, src.len);

    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);
    if (old_src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &old_src_byte);
    }

    if (old_src.len > 0) {
        assert(buf.buffer == old_src.ptr);
        assert(buf.buffer != NULL);
        assert(AWS_MEM_IS_WRITABLE(buf.buffer, buf.capacity));
    } else {
        assert(buf.buffer == NULL);
    }

    assert(buf.len == old_src.len);
    assert(buf.capacity == old_src.len);
    assert(buf.allocator == NULL);

    assert(aws_byte_cursor_is_valid(&src));
    assert(aws_byte_buf_is_valid(&buf));
}
