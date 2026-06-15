#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf dest;
    struct aws_byte_cursor cursor;

    /* nondeterministic length bounded by MAX_BUFFER_SIZE */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* allocate source buffer and fill with nondeterministic data */
    uint8_t *src_buf = malloc(len);
    __CPROVER_assume(src_buf != NULL || len == 0);
    if (src_buf) {
        for (size_t i = 0; i < len; ++i) {
            src_buf[i] = nondet_uint8_t();
        }
    }

    cursor.ptr = src_buf;
    cursor.len = len;

    /* structural preconditions */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* snapshot of source data for frame condition */
    uint8_t *src_snapshot = NULL;
    if (len > 0) {
        src_snapshot = malloc(len);
        __CPROVER_assume(src_snapshot != NULL);
        memcpy(src_snapshot, src_buf, len);
    }

    /* invoke the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, cursor);

    /* post‑conditions */

    if (len != 0) {
        if (result == AWS_OP_SUCCESS) {
            /* successful allocation */
            assert(dest.buffer != NULL);
            assert(dest.len == len);
            assert(dest.capacity == len);
            assert(dest.allocator == allocator);
            /* contents must be copied exactly */
            assert(memcmp(dest.buffer, cursor.ptr, len) == 0);
        } else {
            /* allocation failure */
            assert(result == AWS_OP_ERR);
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == NULL);
        }
    } else {
        /* zero‑length source – always succeeds, no allocation */
        assert(result == AWS_OP_SUCCESS);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
    }

    /* source buffer must remain unchanged */
    if (len > 0) {
        assert(memcmp(src_buf, src_snapshot, len) == 0);
    }

    return 0;
}
