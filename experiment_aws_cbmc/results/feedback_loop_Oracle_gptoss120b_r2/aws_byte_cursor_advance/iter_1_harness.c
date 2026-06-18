#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_cursor_advance_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* --------------------------------------------------------------------
     *  Create a buffer that the cursor will reference.
     * -------------------------------------------------------------------- */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= 64);               /* keep the model small */

    uint8_t *buffer = NULL;
    if (capacity > 0) {
        buffer = aws_mem_acquire(alloc, capacity);
        __CPROVER_assume(buffer != NULL);
        /* fill with nondet data */
        for (size_t i = 0; i < capacity; ++i) {
            buffer[i] = nondet_uint8_t();
        }
    }

    /* --------------------------------------------------------------------
     *  Build a valid aws_byte_cursor that points into the buffer.
     * -------------------------------------------------------------------- */
    struct aws_byte_cursor cursor;
    cursor.ptr = buffer;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= capacity);
    cursor.len = len;

    /* Preserve the original state for later checks */
    struct aws_byte_cursor old_cursor = cursor;
    uint8_t *old_buffer = NULL;
    if (capacity > 0) {
        old_buffer = malloc(capacity);
        __CPROVER_assume(old_buffer != NULL);
        memcpy(old_buffer, buffer, capacity);
    }

    /* --------------------------------------------------------------------
     *  Nondeterministic advance length.
     * -------------------------------------------------------------------- */
    size_t adv_len = nondet_size_t();

    /* --------------------------------------------------------------------
     *  Call the function under test.
     * -------------------------------------------------------------------- */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, adv_len);

    /* --------------------------------------------------------------------
     *  Post‑condition checks.
     * -------------------------------------------------------------------- */
    /* 1. Validity predicates */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&rv));

    /* 2. Return‑value / error‑code correctness and cursor mutation */
    if (old_cursor.len > (SIZE_MAX >> 1) ||
        adv_len > (SIZE_MAX >> 1) ||
        adv_len > old_cursor.len) {
        /* Failure case – function should return an empty cursor and leave
         * the input cursor unchanged. */
        assert(rv.ptr == NULL);
        assert(rv.len == 0);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    } else {
        /* Success case – rv points to the original start, length = adv_len,
         * and the input cursor is advanced accordingly. */
        assert(rv.ptr == old_cursor.ptr);
        assert(rv.len == adv_len);
        if (old_cursor.ptr == NULL) {
            assert(cursor.ptr == NULL);
        } else {
            assert(cursor.ptr == (uint8_t *)old_cursor.ptr + adv_len);
        }
        assert(cursor.len == old_cursor.len - adv_len);
    }

    /* 3. Frame condition – the underlying buffer must not be modified. */
    if (capacity > 0) {
        assert(memcmp(buffer, old_buffer, capacity) == 0);
    }

    /* --------------------------------------------------------------------
     *  Clean‑up.
     * -------------------------------------------------------------------- */
    if (buffer) {
        aws_mem_release(alloc, buffer);
    }
    if (old_buffer) {
        free(old_buffer);
    }

    return 0;
}
