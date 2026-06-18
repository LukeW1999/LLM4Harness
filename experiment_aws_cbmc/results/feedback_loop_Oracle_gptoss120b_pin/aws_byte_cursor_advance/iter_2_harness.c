#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

/* nondet helpers provided by CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_cursor_advance_harness(void) {
    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* ----- set up original cursor ----- */
    struct aws_byte_cursor cursor;
    size_t orig_len = nondet_size_t();
    __CPROVER_assume(orig_len <= 1024);               /* bound allocation size */

    uint8_t *orig_buf = NULL;
    if (orig_len > 0) {
        orig_buf = aws_mem_acquire(allocator, orig_len);
        __CPROVER_assume(orig_buf != NULL);
        for (size_t i = 0; i < orig_len; ++i) {
            orig_buf[i] = nondet_uint8_t();
        }
        cursor.ptr = orig_buf;
        cursor.len = orig_len;
    } else {
        cursor.ptr = NULL;
        cursor.len = 0;
    }

    /* snapshot of the original memory region */
    uint8_t *mem_snapshot = NULL;
    if (orig_len > 0) {
        mem_snapshot = malloc(orig_len);
        __CPROVER_assume(mem_snapshot != NULL);
        memcpy(mem_snapshot, orig_buf, orig_len);
    }

    /* ----- nondet advance length ----- */
    size_t adv_len = nondet_size_t();
    __CPROVER_assume(adv_len <= 1024);               /* bound advance length */

    /* keep a copy of the pre‑call cursor for post‑condition checks */
    struct aws_byte_cursor pre_cursor = cursor;

    /* ----- call function under test ----- */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, adv_len);

    /* ----- post‑conditions ----- */

    /* 1. Return value / error code correctness */
    if (pre_cursor.len > (SIZE_MAX >> 1) ||
        adv_len > (SIZE_MAX >> 1) ||
        adv_len > pre_cursor.len) {
        /* out‑of‑bounds or overflow case */
        assert(rv.ptr == NULL);
        assert(rv.len == 0);
        /* cursor must be unchanged */
        assert(cursor.ptr == pre_cursor.ptr);
        assert(cursor.len == pre_cursor.len);
    } else {
        /* successful advance */
        assert(rv.ptr == pre_cursor.ptr);
        assert(rv.len == adv_len);
        if (pre_cursor.ptr == NULL) {
            assert(cursor.ptr == NULL);
        } else {
            assert(cursor.ptr == (uint8_t *)pre_cursor.ptr + adv_len);
        }
        assert(cursor.len == pre_cursor.len - adv_len);
    }

    /* 3. Memory frame condition: original buffer contents must be unchanged */
    if (orig_len > 0) {
        assert(memcmp(mem_snapshot, orig_buf, orig_len) == 0);
    }

    /* ----- clean‑up ----- */
    if (orig_len > 0) {
        aws_mem_release(allocator, orig_buf);
    }
    free(mem_snapshot);
}
