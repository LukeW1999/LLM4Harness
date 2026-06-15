#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest does not need an allocated buffer before the call */

    /* 2. Save old state */
    struct aws_byte_buf old_src = src;
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    struct aws_allocator *allocator = aws_default_allocator();
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* 4. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* src must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);

        /* dest must have the provided allocator */
        assert(dest.allocator == allocator);

        if (old_src.buffer == NULL) {
            /* When src has no buffer, dest is zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* When src has a buffer, dest gets a new allocation */
            assert(dest.buffer != NULL);
            assert(dest.capacity == old_src.capacity);
            assert(dest.len == old_src.len);
            /* The contents of the new buffer must match src's contents up to src->len */
            assert_bytes_match(old_src.buffer, dest.buffer, old_src.len);
        }
    } else {
        /* Failure path: dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must remain unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    }

    /* 5. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
