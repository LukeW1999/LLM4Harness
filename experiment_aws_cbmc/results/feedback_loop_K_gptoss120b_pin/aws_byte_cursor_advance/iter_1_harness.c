#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

/* maximum size for the concrete buffer we allocate (kept small for CBMC) */
#define MAX_BUF_SIZE 256U

void aws_byte_cursor_advance_harness(void) {
    /* allocator – default (used only for the proof, not by the function under test) */
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondeterministic original length of the cursor */
    size_t orig_len = nondet_size_t();
    __CPROVER_assume(orig_len <= MAX_BUF_SIZE);

    /* allocate a concrete buffer if length > 0 */
    uint8_t *buf = NULL;
    if (orig_len > 0) {
        buf = malloc(orig_len);
        __CPROVER_assume(buf != NULL);
        /* fill the buffer with nondeterministic data */
        for (size_t i = 0; i < orig_len; ++i) {
            buf[i] = nondet_uint8_t();
        }
    }

    /* make a copy of the buffer contents for later comparison */
    uint8_t *buf_copy = NULL;
    if (orig_len > 0) {
        buf_copy = malloc(orig_len);
        __CPROVER_assume(buf_copy != NULL);
        memcpy(buf_copy, buf, orig_len);
    }

    /* initialise the cursor */
    struct aws_byte_cursor cursor;
    cursor.ptr = buf;
    cursor.len = orig_len;

    /* assume the cursor is valid (the conditions above guarantee it) */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* keep a copy of the pre‑state of the cursor */
    struct aws_byte_cursor old_cursor = cursor;
    uint8_t *old_ptr = cursor.ptr;
    size_t old_len = cursor.len;

    /* nondeterministic advance length */
    size_t len = nondet_size_t();

    /* call the function under test */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, len);

    /* ---- post‑condition checks ---- */

    /* rv must be a valid cursor */
    assert(aws_byte_cursor_is_valid(&rv));

    /* overflow / out‑of‑bounds case */
    if (old_len > (SIZE_MAX >> 1) || len > (SIZE_MAX >> 1) || len > old_len) {
        assert(rv.ptr == NULL);
        assert(rv.len == 0);
        /* original cursor unchanged */
        assert(cursor.ptr == old_ptr);
        assert(cursor.len == old_len);
    } else {
        /* successful advance */
        assert(rv.ptr == old_ptr);
        assert(rv.len == len);
        assert(cursor.ptr == (old_ptr == NULL ? NULL : old_ptr + len));
        assert(cursor.len == old_len - len);
    }

    /* ---- frame condition: buffer contents unchanged ---- */
    if (orig_len > 0) {
        assert(memcmp(buf, buf_copy, orig_len) == 0);
    }

    /* free allocated memory */
    free(buf);
    free(buf_copy);

    return 0;
}
