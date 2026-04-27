#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define NUM_ARGS 2
#define MAX_BUFFER_SIZE 8

void aws_byte_buf_cat_harness(void) {
    /* Set up destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Set up source buffers */
    struct aws_byte_buf src1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    struct aws_byte_buf src2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* Save old state */
    size_t old_len = dest.len;
    size_t old_capacity = dest.capacity;
    struct aws_allocator *old_allocator = dest.allocator;
    uint8_t *old_buffer = dest.buffer;

    size_t src1_old_len = src1.len;
    size_t src2_old_len = src2.len;

    /* Call function under test */
    int result = aws_byte_buf_cat(&dest, NUM_ARGS, &src1, &src2);

    /* Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));

    /* Fields that never change */
    assert(dest.capacity == old_capacity);
    assert(dest.allocator == old_allocator);
    assert(dest.buffer == old_buffer);

    /* Source buffers must not be modified */
    assert(src1.len == src1_old_len);
    assert(src2.len == src2_old_len);

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest->len increased by sum of src lens */
        assert(dest.len == old_len + src1_old_len + src2_old_len);
        /* dest->len must not exceed capacity */
        assert(dest.len <= dest.capacity);
    } else {
        /* On failure, result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
        /* dest->len may have been partially updated */
        assert(dest.len >= old_len);
        assert(dest.len <= dest.capacity);
    }
}
