#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_init_copy_harness() {
    /* Declare destination (uninitialized, will be written by the function) */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf src;

    /* Bound and initialize source buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old source state to verify immutability */
    struct aws_byte_buf old_src = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postcondition 1: src must remain completely unchanged */
    assert(aws_byte_buf_is_valid(&src));
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    if (src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* Postcondition 2: dest must always be valid after call */
    assert(aws_byte_buf_is_valid(&dest));

    /* Postcondition 3: Success path */
    if (result == AWS_OP_SUCCESS) {
        /* Allocator must match the parameter */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* Source has no buffer: dest is zeroed with allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Source has a buffer: dest gets a new allocation with copied contents */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Postcondition 4: Failure path (allocation failed) */
        /* This can only happen when src has a non-NULL buffer */
        assert(src.buffer != NULL);
        /* On failure, dest is completely zeroed */
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
    }
}
