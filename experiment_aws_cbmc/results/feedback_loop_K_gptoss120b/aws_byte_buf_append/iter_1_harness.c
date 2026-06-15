#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);
bool nondet_bool(void);

void aws_byte_buf_append_harness(void) {
    /* Allocate and nondeterministically initialize the destination buffer */
    struct aws_byte_buf to;
    to.allocator = aws_default_allocator();

    to.capacity = nondet_size_t();
    __CPROVER_assume(to.capacity <= SIZE_MAX / sizeof(uint8_t)); /* avoid overflow */

    if (to.capacity > 0) {
        to.buffer = (uint8_t *)malloc(to.capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }

    to.len = nondet_size_t();
    __CPROVER_assume(to.len <= to.capacity);

    /* Snapshot of fields for frame reasoning */
    struct aws_allocator *old_allocator = to.allocator;
    size_t old_capacity = to.capacity;
    uint8_t *old_buffer = to.buffer;
    size_t old_len = to.len;

    /* Snapshot of buffer contents up to old_len */
    uint8_t *old_buf_contents = NULL;
    if (old_len > 0) {
        old_buf_contents = (uint8_t *)malloc(old_len);
        __CPROVER_assume(old_buf_contents != NULL);
        memcpy(old_buf_contents, to.buffer, old_len);
    }

    /* Allocate and nondeterministically initialize the source cursor */
    struct aws_byte_cursor from;
    from.len = nondet_size_t();
    if (from.len > 0) {
        from.ptr = (const uint8_t *)malloc(from.len);
        __CPROVER_assume(from.ptr != NULL);
    } else {
        from.ptr = NULL;
    }

    /* Assume validity predicates */
    __CPROVER_assume(to.buffer != NULL || to.capacity == 0);
    __CPROVER_assume(from.ptr != NULL || from.len == 0);

    /* Call the function under verification */
    int ret = aws_byte_buf_append(&to, &from);

    /* Postcondition: return value */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_ERROR_DEST_COPY_TOO_SMALL);

    /* Postcondition: destination buffer validity */
    assert(to.allocator == aws_default_allocator());
    assert(to.capacity == old_capacity);
    assert(to.buffer == old_buffer);
    assert(to.len <= to.capacity);
    assert(to.capacity == 0 || to.buffer != NULL);

    /* Postcondition: source cursor validity (unchanged) */
    assert(from.ptr != NULL || from.len == 0);

    /* Postcondition: length invariants */
    if (to.capacity - old_len >= from.len) {
        assert(ret == AWS_OP_SUCCESS);
        assert(to.len == old_len + from.len);
    } else {
        assert(ret == AWS_ERROR_DEST_COPY_TOO_SMALL);
        assert(to.len == old_len);
        /* Buffer contents must be unchanged when copy fails */
        if (old_len > 0) {
            assert(memcmp(to.buffer, old_buf_contents, old_len) == 0);
        }
    }

    /* Clean up */
    if (to.buffer) free(to.buffer);
    if (from.ptr) free((void *)from.ptr);
    if (old_buf_contents) free(old_buf_contents);

    return 0;
}
