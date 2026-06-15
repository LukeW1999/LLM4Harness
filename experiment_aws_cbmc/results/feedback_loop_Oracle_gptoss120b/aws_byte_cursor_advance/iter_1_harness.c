#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Helper to get a nondeterministic size_t */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_cursor_advance_harness(void) {
    /* Allocate a buffer for the cursor */
    size_t buf_len = nondet_size_t();
    /* Keep the size reasonable for CBMC */
    __CPROVER_assume(buf_len <= 1024);
    /* Ensure we are not in the overflow region */
    __CPROVER_assume(buf_len <= (SIZE_MAX >> 1));

    uint8_t *buf = (uint8_t *)aws_mem_acquire(aws_default_allocator(), buf_len);
    __CPROVER_assume(buf != NULL || buf_len == 0);

    /* Fill the buffer with nondeterministic data */
    for (size_t i = 0; i < buf_len; ++i) {
        buf[i] = nondet_uint8_t();
    }

    /* Keep a copy of the original buffer to check frame conditions */
    uint8_t *buf_copy = (uint8_t *)malloc(buf_len);
    __CPROVER_assume(buf_copy != NULL || buf_len == 0);
    if (buf_len > 0) {
        memcpy(buf_copy, buf, buf_len);
    }

    /* Build a valid aws_byte_cursor */
    struct aws_byte_cursor cursor;
    cursor.ptr = buf;
    cursor.len = buf_len;

    /* Save old state */
    const uint8_t *old_ptr = cursor.ptr;
    size_t old_len = cursor.len;

    /* Nondeterministic advance length */
    size_t adv_len = nondet_size_t();
    __CPROVER_assume(adv_len <= 1024);
    __CPROVER_assume(adv_len <= (SIZE_MAX >> 1));

    /* Call the function under test */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, adv_len);

    /* Post‑condition 1: return value / error code correctness */
    bool success = (old_len <= (SIZE_MAX >> 1)) && (adv_len <= (SIZE_MAX >> 1)) && (adv_len <= old_len);
    if (success) {
        /* Successful advance */
        assert(rv.ptr == old_ptr);
        assert(rv.len == adv_len);
        assert(cursor.ptr == old_ptr + adv_len);
        assert(cursor.len == old_len - adv_len);
    } else {
        /* Failed advance */
        assert(rv.ptr == NULL);
        assert(rv.len == 0);
        assert(cursor.ptr == old_ptr);
        assert(cursor.len == old_len);
    }

    /* Post‑condition 2: output cursor validity */
    assert(aws_byte_cursor_is_valid(&rv));

    /* Post‑condition 3: input cursor validity (frame condition) */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Post‑condition 4: memory frame condition – buffer contents unchanged */
    if (buf_len > 0) {
        assert(memcmp(buf, buf_copy, buf_len) == 0);
    }

    /* Clean up */
    if (buf) {
        aws_mem_release(aws_default_allocator(), buf);
    }
    free(buf_copy);

    return 0;
}
