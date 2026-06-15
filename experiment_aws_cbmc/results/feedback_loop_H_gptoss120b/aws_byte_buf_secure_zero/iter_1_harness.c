#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>

void aws_byte_buf_init_copy_harness(void) {
    /* allocator – must be non‑NULL */
    struct aws_allocator *alloc;
    __CPROVER_assume(alloc != NULL);

    /* source buffer */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* nondeterministically decide whether src is empty (buffer == NULL) */
    if (nondet_bool()) {
        src.buffer   = NULL;
        src.capacity = 0;
        src.len      = 0;
    } else {
        /* keep the allocated buffer, but make len a nondet value within capacity */
        src.len = nondet_size_t();
        __CPROVER_assume(src.len <= src.capacity);
    }
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* destination buffer (output) */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    /* dest may contain arbitrary initial contents – we only need it to be a valid struct */
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* save old states for immutability checks */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src  = src;

    /* optionally save a byte from src for later content comparison */
    struct store_byte_from_buffer src_byte;
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte);
    }

    /* call the function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the one passed in */
        assert(dest.allocator == alloc);

        /* length and capacity must mirror the source */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        /* buffer must be non‑NULL when source had a buffer, otherwise it is NULL */
        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            /* contents must be identical for the length of the source */
            assert_bytes_match(src.buffer, dest.buffer, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* on failure the destination is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* source must remain unchanged regardless of outcome */
    assert(src.buffer   == old_src.buffer);
    assert(src.len      == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }

    /* validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
