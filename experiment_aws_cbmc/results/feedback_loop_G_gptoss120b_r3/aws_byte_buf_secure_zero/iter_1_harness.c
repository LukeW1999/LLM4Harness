#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>

#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Non-deterministically initialize src and bound it */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Save old states */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 5. Global validity invariant */
    assert(aws_byte_buf_is_valid(&dest));

    /* 6. Postconditions when the call succeeds */
    if (result == AWS_OP_SUCCESS) {
        /* allocator is set to the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* When source has no buffer, destination is an empty buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Destination must have an allocated buffer */
            assert(dest.buffer != NULL);
            /* Length and capacity are copied from the source */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The first src.len bytes are identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* 7. Postconditions when the call fails (allocation failure) */
        /* Destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 8. Source buffer must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
}
