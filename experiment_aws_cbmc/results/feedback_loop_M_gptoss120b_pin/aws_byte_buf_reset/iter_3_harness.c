#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare and bound input structures */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest; /* output, uninitialized */

    struct aws_allocator *alloc = aws_default_allocator();

    /* 2. Save old state of src (immutable) */
    struct aws_byte_buf old_src = src;

    /* 3. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 4. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator is set to the one we passed */
        assert(dest.allocator == alloc);

        /* capacity and length mirror src */
        assert(dest.capacity == src.capacity);
        assert(dest.len == src.len);

        if (src.buffer == NULL) {
            /* when src has no buffer, dest should also have none */
            assert(dest.buffer == NULL);
        } else {
            /* dest must have a non‑NULL buffer and contain a copy of src's data */
            assert(dest.buffer != NULL);
            assert_byte_buf_eq(&dest, &src);
        }

        /* src must remain unchanged */
        assert_byte_buf_eq(&src, &old_src);
    } else {
        /* On allocation failure dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);

        /* src must remain unchanged */
        assert_byte_buf_eq(&src, &old_src);
    }

    /* 5. Validity invariants for src */
    assert(aws_byte_buf_is_valid(&src));
}
