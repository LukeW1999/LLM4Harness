#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/* Stub for aws_byte_cursor_from_buf */
struct aws_byte_cursor aws_byte_cursor_from_buf(const struct aws_byte_buf *const buf) {
    struct aws_byte_cursor cursor;
    cursor.ptr = buf->buffer;
    cursor.len = buf->len;
    return cursor;
}

/* Stub for aws_byte_buf_append that models its contract */
int aws_byte_buf_append(struct aws_byte_buf *to, const struct aws_byte_cursor *from) {
    /* Nondeterministic return value */
    int ret;
    __CPROVER_assume(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    if (ret == AWS_OP_SUCCESS) {
        /* On success, to->len increases by from->len, and to->len <= to->capacity */
        size_t new_len = to->len + from->len;
        __CPROVER_assume(new_len <= to->capacity);
        to->len = new_len;
    }
    /* On failure, to->len is unchanged */
    return ret;
}

void aws_byte_buf_cat_harness() {
    /* Bounded number of arguments: we use exactly 2 source buffers */
    const size_t number_of_args = 2;

    /* Declare and initialize destination buffer */
    struct aws_byte_buf dest;
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Declare and initialize source buffers */
    struct aws_byte_buf src1, src2;
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    /* Call the function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args, &src1, &src2);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest->len increased by sum of source lengths */
        size_t total_len = old_dest.len + old_src1.len + old_src2.len;
        assert(dest.len == total_len);
    } else {
        /* On failure, dest->len may have been partially updated */
        /* It must be at least old_dest.len and at most old_dest.len + old_src1.len + old_src2.len */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= old_dest.len + old_src1.len + old_src2.len);
    }

    /* Unchanged fields for dest */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    /* Source buffers must be unchanged */
    assert(src1.len == old_src1.len);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);
    assert(src1.buffer == old_src1.buffer);

    assert(src2.len == old_src2.len);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.allocator == old_src2.allocator);
    assert(src2.buffer == old_src2.buffer);

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
}
