#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest is uninitialized; it will be initialized by the function under test */

    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be stored in dest */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* src is a null buffer: dest should be a zeroed buffer (except allocator) */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* src has a buffer: dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* contents must be copied */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure the function zeroes the destination struct */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* allocator pointer is an input, it does not change */
    assert(allocator != NULL);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
