#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdarg.h>
#include <assert.h>

/* We test with a fixed small number of args to keep state space bounded */
#define NUM_ARGS 2

void aws_byte_buf_cat_harness(void) {
    /* 1. Set up destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Set up source buffers */
    struct aws_byte_buf src0;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src0);
    __CPROVER_assume(aws_byte_buf_is_valid(&src0));

    struct aws_byte_buf src1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    /* 3. Save old state */
    struct aws_byte_buf old_dest = dest;
    size_t old_dest_len = dest.len;

    /* 4. Call function under test */
    int result = aws_byte_buf_cat(&dest, NUM_ARGS, &src0, &src1);

    /* 5. Assert postconditions */

    /* Fields that must never change regardless of result */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest.len increased by sum of src lengths */
        assert(dest.len == old_dest_len + src0.len + src1.len);
        /* len must not exceed capacity */
        assert(dest.len <= dest.capacity);
    } else {
        /* On failure, dest.len may have been partially updated (first append
         * succeeded but second failed), OR dest.len is unchanged if first
         * append failed. Either way, dest.len <= old_dest_len + src0.len + src1.len.
         * The key invariant is that dest remains valid. */
        /* dest.len should be >= old_dest_len (partial progress possible) */
        assert(dest.len >= old_dest_len);
        assert(dest.len <= dest.capacity);
    }

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src0));
    assert(aws_byte_buf_is_valid(&src1));
}
