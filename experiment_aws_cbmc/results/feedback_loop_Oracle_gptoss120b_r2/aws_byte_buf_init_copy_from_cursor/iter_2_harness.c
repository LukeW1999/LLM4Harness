#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* destination buffer (output) */
    struct aws_byte_buf dest = {0};

    /* source cursor */
    struct aws_byte_cursor src;

    /* allocate a backing array for the cursor */
    uint8_t *src_buf = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(src_buf != NULL);

    /* nondet length bounded by MAX_BUFFER_SIZE */
    src.len = nondet_size_t();
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);

    src.ptr = src_buf;

    /* make structural assumptions required by the function */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* keep a copy of the source memory to check for modifications */
    uint8_t *src_buf_snapshot = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(src_buf_snapshot != NULL);
    memcpy(src_buf_snapshot, src.ptr, src.len);

    /* keep a snapshot of the destination struct before the call */
    struct aws_byte_buf dest_snapshot;
    memcpy(&dest_snapshot, &dest, sizeof(dest));

    /* call the function under verification */
    int rv = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* ---------- post‑condition checks ---------- */

    /* 1. Return value / error code correctness */
    if (src.len == 0) {
        /* zero‑length source must succeed and produce a zero‑length buffer */
        assert(rv == AWS_OP_SUCCESS);
    } else {
        /* when allocation fails the function returns AWS_OP_ERR */
        if (dest.buffer == NULL) {
            assert(rv == AWS_OP_ERR);
        } else {
            assert(rv == AWS_OP_SUCCESS);
        }
    }

    /* 2. Output buffer length / capacity invariants */
    if (rv == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* on error the destination must remain zero‑initialized */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* 3. Memory not modified beyond the function's contract */

    /* source buffer must be unchanged */
    if (src.len > 0) {
        assert(memcmp(src.ptr, src_buf_snapshot, src.len) == 0);
    }

    /* destination buffer contents must match source when successful */
    if (rv == AWS_OP_SUCCESS && src.len > 0) {
        assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
    }

    /* fields of dest that are not part of the contract must remain as they were
       (the implementation zero‑initialises dest at entry) */
    assert(dest_snapshot.len == 0);
    assert(dest_snapshot.capacity == 0);
    assert(dest_snapshot.buffer == NULL);
    assert(dest_snapshot.allocator == NULL);

    /* clean up */
    free(src_buf);
    free(src_buf_snapshot);
    if (dest.buffer != NULL) {
        aws_mem_release(allocator, dest.buffer);
    }
}
