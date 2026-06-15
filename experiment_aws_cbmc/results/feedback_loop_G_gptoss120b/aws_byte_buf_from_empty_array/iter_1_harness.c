#include <aws/common/byte_buf.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc;

    /* allocator must be non‑NULL for the precondition */
    __CPROVER_assume(alloc != NULL);

    /* bound the buffers */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));

    /* allocate the internal buffer members for src (dest will be (re)allocated inside the function) */
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* src must be a valid byte buffer before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 2. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest must reflect a copy of src */
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        if (src.buffer == NULL) {
            /* when src has no buffer, dest must also have no buffer */
            assert(dest.buffer == NULL);
        } else {
            /* when src has a buffer, dest must have a newly allocated buffer */
            assert(dest.buffer != NULL);
        }

        /* src must be unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    } else {
        /* on allocation failure the function zeroes dest */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must be unchanged */
        assert(src.len == old_src.len);
        assert(src.capacity == old_src.capacity);
        assert(src.buffer == old_src.buffer);
        assert(src.allocator == old_src.allocator);
    }

    /* 5. Fields of dest that are not mentioned as changing must retain their old values
       (except those explicitly set above). In this function all fields are either set
       on success or cleared on failure, so no additional unchanged‑field asserts are needed. */

    /* 6. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
