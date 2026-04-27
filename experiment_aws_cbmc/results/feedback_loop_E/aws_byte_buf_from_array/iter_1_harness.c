#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;

    /* parameters */
    struct aws_allocator *allocator = aws_default_allocator(); /* Precondition: allocator is non-null */

    /* bound and allocate src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* save old state of src */
    struct aws_byte_buf old_src = src;

    /* call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* assertions */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        assert_bytes_match(dest.buffer, src.buffer, src.len);
    } else {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
    }

    /* unchanged fields */
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
