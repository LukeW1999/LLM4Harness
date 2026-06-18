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
    struct aws_byte_buf dest;
    struct aws_byte_buf src;

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest's buffer is not required to be allocated beforehand */

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* 3. Call function under test */
    struct aws_allocator *alloc = aws_default_allocator();
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Postconditions for both success and failure */
    /* Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the one passed in */
        assert(dest.allocator == alloc);

        /* length and capacity must match source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        if (src.buffer == NULL) {
            /* When source buffer is NULL, destination buffer must also be NULL */
            assert(dest.buffer == NULL);
        } else {
            /* Destination must have a non‑NULL buffer */
            assert(dest.buffer != NULL);
            /* The contents of the buffer must be identical for src.len bytes */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Fields that must remain unchanged regardless of result */
    /* Destination fields not mentioned as changing are already covered above */
    /* Source must be unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
}
