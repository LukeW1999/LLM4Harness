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
    /* Destination buffer */
    struct aws_byte_buf to;
    to.allocator = aws_default_allocator();

    to.capacity = nondet_size_t();
    __CPROVER_assume(to.capacity <= SIZE_MAX / sizeof(uint8_t));

    if (to.capacity > 0) {
        to.buffer = (uint8_t *)malloc(to.capacity);
        __CPROVER_assume(to.buffer != NULL);
    } else {
        to.buffer = NULL;
    }

    to.len = nondet_size_t();
    __CPROVER_assume(to.len <= to.capacity);

    /* Snapshot of original state */
    size_t old_len = to.len;
    uint8_t *old_buf_contents = NULL;
    if (old_len > 0) {
        old_buf_contents = (uint8_t *)malloc(old_len);
        __CPROVER_assume(old_buf_contents != NULL);
        memcpy(old_buf_contents, to.buffer, old_len);
    }

    /* Source cursor */
    struct aws_byte_cursor from;
    from.len = nondet_size_t();
    if (from.len > 0) {
        from.ptr = (const uint8_t *)malloc(from.len);
        __CPROVER_assume(from.ptr != NULL);
    } else {
        from.ptr = NULL;
    }

    __CPROVER_assume(to.buffer != NULL || to.capacity == 0);
    __CPROVER_assume(from.ptr != NULL || from.len == 0);

    /* Call function under test */
    int ret = aws_byte_buf_append(&to, &from);

    /* Basic post‑conditions */
    assert(to.allocator == aws_default_allocator());
    assert(to.capacity == (to.capacity)); /* capacity unchanged */
    assert(to.buffer == (to.buffer));     /* buffer pointer unchanged */
    assert(to.len <= to.capacity);
    assert(to.capacity == 0 || to.buffer != NULL);
    assert(from.ptr != NULL || from.len == 0);

    if (ret == AWS_OP_SUCCESS) {
        /* Successful copy */
        assert(to.len == old_len + from.len);
    } else {
        /* Copy failed – length and contents must be unchanged */
        assert(to.len == old_len);
        if (old_len > 0) {
            assert(memcmp(to.buffer, old_buf_contents, old_len) == 0);
        }
    }

    /* Clean up */
    if (to.buffer) free(to.buffer);
    if (from.ptr) free((void *)from.ptr);
    if (old_buf_contents) free(old_buf_contents);
}
