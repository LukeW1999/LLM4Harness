#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 8
#endif

#ifndef NUM_ARGS
#    define NUM_ARGS 2
#endif

void aws_byte_buf_cat_harness(void) {
    /* 1. Set up destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Set up source buffers (fixed number for bounded verification) */
    struct aws_byte_buf buf0;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf0);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf0));

    struct aws_byte_buf buf1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));

    /* 3. Save old state */
    struct aws_byte_buf old_dest = dest;
    size_t old_len = dest.len;

    /* 4. Call function under test */
    int result = aws_byte_buf_cat(&dest, NUM_ARGS, &buf0, &buf1);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest->len should have increased by the sum of source lens */
        assert(dest.len == old_len + buf0.len + buf1.len);
        /* capacity and allocator and buffer pointer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
    } else {
        /* On failure, dest may have been partially modified (append stops on first error)
         * but capacity, allocator, and buffer pointer should remain unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        /* len may have been partially updated before failure, but should not exceed capacity */
        assert(dest.len <= dest.capacity);
    }

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&buf0));
    assert(aws_byte_buf_is_valid(&buf1));
}
