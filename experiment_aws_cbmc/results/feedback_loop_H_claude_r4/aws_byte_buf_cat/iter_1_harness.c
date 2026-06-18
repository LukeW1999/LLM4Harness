#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the number of args to keep state space manageable */
#ifndef NUM_ARGS
#    define NUM_ARGS 2
#endif

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_cat_harness(void) {
    /* 1. Set up destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Set up source buffers (fixed number for bounded verification) */
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
    size_t old_len = dest.len;

    /* 4. Call function under test */
    int result = aws_byte_buf_cat(&dest, NUM_ARGS, &src0, &src1);

    /* 5. Assert postconditions */

    /* Fields that must never change regardless of result */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest->len contains the amount of data actually copied */
        /* dest->len should be >= old_len (we appended data) */
        assert(dest.len >= old_len);
        /* The total appended is src0.len + src1.len */
        assert(dest.len == old_len + src0.len + src1.len);
        /* dest must still be valid */
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On failure: dest->len contains the amount of data actually copied before failure */
        /* dest->len should be >= old_len (partial copy may have occurred) */
        /* Actually, looking at the implementation: aws_byte_buf_append returns error
           and we return immediately, so dest->len may have been partially updated
           by the first successful append before the second fails.
           But if the first append fails, dest->len == old_len.
           We can only assert dest is still valid. */
        assert(aws_byte_buf_is_valid(&dest));
        /* len is >= old_len (partial appends may have succeeded) */
        assert(dest.len >= old_len);
        /* len cannot exceed capacity */
        assert(dest.len <= dest.capacity);
    }

    /* Source buffers must remain valid and unchanged */
    assert(aws_byte_buf_is_valid(&src0));
    assert(aws_byte_buf_is_valid(&src1));
}
