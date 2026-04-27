#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = aws_default_allocator();

    /* bounding and assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        assert_bytes_match(dest.buffer, src.buffer, src.len);
    } else {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
    }

    /* unchanged fields */
    assert(src.allocator == old_src.allocator);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);

    /* allocator is unchanged on both paths */
    assert(dest.allocator == allocator);
}
