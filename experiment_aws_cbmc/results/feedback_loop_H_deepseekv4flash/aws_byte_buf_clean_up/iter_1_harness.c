#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    struct aws_byte_buf src;

    /* Preconditions from Doxygen and implementation */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* Call function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest is valid and initialized */
        assert(aws_byte_buf_is_valid(&dest));
        if (src.buffer == NULL) {
            /* src buffer is null: dest has null buffer, len=0, capacity=0 */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* src buffer non-null: dest is a copy of src with new allocator */
            assert(dest.allocator == allocator);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer); /* new allocation */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Failure: dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must remain unchanged */
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(aws_byte_buf_is_valid(&src));
}
