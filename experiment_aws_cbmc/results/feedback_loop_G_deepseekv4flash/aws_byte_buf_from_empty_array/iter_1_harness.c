#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

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
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state */
    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_storage;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_storage);
    }

    /* Call function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* Success: dest is valid and initialized */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
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
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    if (src.buffer != NULL && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_storage);
    }
    assert(aws_byte_buf_is_valid(&src));
}
