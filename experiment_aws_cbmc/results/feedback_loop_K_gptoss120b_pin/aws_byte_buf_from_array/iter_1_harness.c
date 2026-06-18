#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_LEN 256U   /* arbitrary bound for CBMC exploration */

void aws_byte_buf_from_array_harness(void) {
    /* ---------- nondeterministic inputs ---------- */
    size_t len;
    __CPROVER_assume(len <= MAX_LEN);

    uint8_t *bytes = NULL;
    uint8_t *bytes_snapshot = NULL;

    if (len > 0) {
        /* allocate a writable buffer */
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);

        /* initialise with nondeterministic contents */
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = (uint8_t) __CPROVER_nondet_uint();
        }

        /* take a snapshot to check the frame condition later */
        bytes_snapshot = malloc(len);
        __CPROVER_assume(bytes_snapshot != NULL);
        memcpy(bytes_snapshot, bytes, len);
    }

    /* ---------- call the function under test ---------- */
    struct aws_byte_buf buf = aws_byte_buf_from_array(bytes, len);

    /* ---------- post‑condition checks ---------- */
    /* validity */
    assert(aws_byte_buf_is_valid(&buf));

    /* length / capacity invariants */
    assert(buf.len == len);
    assert(buf.capacity == len);

    /* allocator must be NULL */
    assert(buf.allocator == NULL);

    /* buffer pointer relationship */
    if (len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == bytes);
    }

    /* ---------- frame condition (no modification of input array) ---------- */
    if (len > 0) {
        assert(memcmp(bytes, bytes_snapshot, len) == 0);
    }

    /* clean up */
    free(bytes);
    free(bytes_snapshot);

    return 0;
}
