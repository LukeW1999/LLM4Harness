#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

/* CBMC harness for aws_byte_buf_init_copy */
void aws_byte_buf_init_copy_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *alloc = nondet_uint8_t() ? (struct aws_allocator *)0x1 : (struct aws_allocator *)0x1; /* non‑NULL */

    /* 2. Bound and initialise src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);

    /* nondet length / capacity respecting validity */
    size_t src_len   = nondet_size_t();
    size_t src_cap   = nondet_size_t();
    __CPROVER_assume(src_len <= src_cap);
    __CPROVER_assume(src_cap <= MAX_BUFFER_SIZE);

    src.len      = src_len;
    src.capacity = src_cap;

    /* allocate / set buffer pointer */
    uint8_t *src_buf = nondet_uint8_t() ? (uint8_t *)0x1 : (uint8_t *)0x1; /* non‑NULL when capacity > 0 */
    __CPROVER_assume((src_cap == 0) ? (src_buf == NULL) : (src_buf != NULL));
    src.buffer = src_buf;

    /* make the buffer readable for src_len bytes */
    __CPROVER_assume(AWS_MEM_IS_READABLE(src.buffer, src.len));

    /* 3. Preserve old state of src (dest is output, no need to preserve) */
    struct aws_byte_buf old_src = src;

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    /* 5. Global post‑condition: dest must always be a valid byte buffer */
    assert(aws_byte_buf_is_valid(&dest));

    /* 6. src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* 7. Result‑dependent post‑conditions */
    if (result == AWS_OP_SUCCESS) {
        /* success when src.buffer is NULL */
        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == alloc);
        } else {
            /* success when src.buffer is non‑NULL */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            assert(dest.allocator == alloc);
            /* the newly allocated buffer must contain a copy of src's data */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* on failure the function zeroes the destination */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* failure can only happen when src.buffer was non‑NULL and allocation failed */
        assert(src.buffer != NULL);
    }

    /* 8. In all cases the validity invariant holds for src as well */
    assert(aws_byte_buf_is_valid(&src));
}
