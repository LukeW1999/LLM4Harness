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

void aws_byte_buf_init_copy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    /* nondet capacity and len, bounded by MAX_BUFFER_SIZE */
    __CPROVER_assume(src.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(src.len <= src.capacity);
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* uninitialized, will be initialized by the function */

    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state of src (dest will be overwritten on failure) */
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator of dest must be the one passed in */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* When source buffer is NULL, dest should be empty and have NULL buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* When source buffer is non‑NULL, dest must have its own allocation */
            assert(dest.buffer != NULL);
            assert(dest.buffer != src.buffer);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* The contents of the first src.len bytes must be identical */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On allocation failure the function zeroes dest */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 5. Fields of src must remain unchanged regardless of result */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* 6. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&src));
}
