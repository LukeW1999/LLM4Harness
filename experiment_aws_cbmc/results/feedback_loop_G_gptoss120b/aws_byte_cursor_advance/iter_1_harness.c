#include <assert.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare structures and nondet allocator */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *alloc = nondet_uint8_t() ? (struct aws_allocator *)0x1 : NULL; /* nondet non‑NULL */
    __CPROVER_assume(alloc != NULL);

    /* 2. Bound and allocate src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* 3. Save old state of src (dest is output, no need to save) */
    struct aws_byte_buf old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        if (src.buffer == NULL) {
            /* src empty → dest empty, allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == alloc);
        } else {
            /* allocation succeeded → dest mirrors src and uses given allocator */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == alloc);
            /* content of the first src.len bytes must be copied */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Failure can only happen when src.buffer != NULL and allocation failed */
        assert(src.buffer != NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 6. src must remain unchanged */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.buffer != NULL && src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* 7. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
