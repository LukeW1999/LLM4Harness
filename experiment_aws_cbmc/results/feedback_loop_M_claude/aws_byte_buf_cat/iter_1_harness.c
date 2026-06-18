#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* We test with a fixed small number of args to keep state space bounded */
#ifndef NUM_ARGS
#    define NUM_ARGS 2
#endif

void aws_byte_buf_cat_harness(void) {
    /* 1. Set up destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Set up source buffers */
    struct aws_byte_buf buf0;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf0);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf0));

#if NUM_ARGS >= 2
    struct aws_byte_buf buf1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));
#endif

    /* 3. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_buf0 = buf0;
#if NUM_ARGS >= 2
    struct aws_byte_buf old_buf1 = buf1;
#endif

    /* 4. Call function under test */
    int result;
#if NUM_ARGS == 1
    result = aws_byte_buf_cat(&dest, 1, &buf0);
#elif NUM_ARGS == 2
    result = aws_byte_buf_cat(&dest, 2, &buf0, &buf1);
#else
    result = aws_byte_buf_cat(&dest, 1, &buf0);
#endif

    /* 5. Assert postconditions */

    /* Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&buf0));
#if NUM_ARGS >= 2
    assert(aws_byte_buf_is_valid(&buf1));
#endif

    /* Fields that never change on dest */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);

    /* Source buffers must not be modified */
    assert(buf0.len == old_buf0.len);
    assert(buf0.capacity == old_buf0.capacity);
    assert(buf0.buffer == old_buf0.buffer);
    assert(buf0.allocator == old_buf0.allocator);
#if NUM_ARGS >= 2
    assert(buf1.len == old_buf1.len);
    assert(buf1.capacity == old_buf1.capacity);
    assert(buf1.buffer == old_buf1.buffer);
    assert(buf1.allocator == old_buf1.allocator);
#endif

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest->len increases by the total bytes appended */
#if NUM_ARGS == 1
        assert(dest.len == old_dest.len + buf0.len);
#elif NUM_ARGS == 2
        assert(dest.len == old_dest.len + buf0.len + buf1.len);
#endif
        /* dest->len must not exceed capacity */
        assert(dest.len <= dest.capacity);
    } else {
        /* On failure (AWS_ERROR_DEST_COPY_TOO_SMALL), dest->len is unchanged */
        assert(dest.len == old_dest.len);
    }
}
