#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = nondet_uint8_t() ? (struct aws_allocator *)0x1 : (struct aws_allocator *)0x2; /* nondet non‑NULL */

    /* 2. Impose bounds and validity on src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Save old state of src (dest will be overwritten) */
    struct aws_byte_buf old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Post‑conditions for success */
    if (result == AWS_OP_SUCCESS) {
        /* allocator must be set to the supplied allocator */
        assert(dest.allocator == alloc);

        if (src.buffer == NULL) {
            /* when src has no buffer, dest is zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* buffer must be allocated */
            assert(dest.buffer != NULL);
            /* length and capacity must match src */
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* contents must be copied */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* 6. Post‑conditions for failure */
        /* dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 7. src must be unchanged regardless of outcome */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.len > 0) {
        /* ensure the bytes themselves were not modified */
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* 8. Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
