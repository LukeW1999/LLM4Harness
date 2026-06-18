#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/math.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/linked_list.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;
    /* dest does not need to be valid before the call; it will be initialized inside the function */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Save old state */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* Save a byte from src.buffer for later content comparison (if buffer exists) */
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.capacity > 0) {
        save_byte_from_array(src.buffer, src.capacity, &src_byte);
    }

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When src has no buffer, dest should be an empty buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
            /* length and capacity must match src */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* contents must be identical for the length of src */
            if (src.len > 0) {
                assert(0 == memcmp(dest.buffer, src.buffer, src.len));
                /* also check that the saved byte matches */
                assert_byte_from_buffer_matches(dest.buffer + src_byte.index, &src_byte);
            }
        }
    } else {
        /* On failure the destination buffer is zeroed */
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

    /* dest fields that are not explicitly set by the function should remain unchanged on failure */
    if (result != AWS_OP_SUCCESS) {
        assert(dest.buffer == old_dest.buffer);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    /* 6. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
