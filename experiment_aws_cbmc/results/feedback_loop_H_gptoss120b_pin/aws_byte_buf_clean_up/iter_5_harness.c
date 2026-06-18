#include <assert.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = aws_default_allocator();

    /* src is a bounded, valid byte buffer */
    __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(src.len <= src.capacity);

    struct aws_byte_buf old_src = src;

    struct store_byte_from_buffer src_bytes;
    save_byte_from_array(src.buffer, src.len, &src_bytes);

    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);
        assert(dest.buffer != NULL);
        assert_bytes_match(dest.buffer, src.buffer, src.len);
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must be unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert_byte_from_buffer_matches(src.buffer, &src_bytes);

    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
