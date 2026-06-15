#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* nondeterministic length, bounded to keep the model tractable */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 64);

    /* allocate a writable buffer when len > 0, otherwise allow NULL */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
        /* fill the buffer with nondeterministic data */
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    /* keep a copy of the original contents to check frame conditions */
    uint8_t *bytes_copy = NULL;
    if (len > 0) {
        bytes_copy = malloc(len);
        __CPROVER_assume(bytes_copy != NULL);
        memcpy(bytes_copy, bytes, len);
    }

    /* call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(bytes, len);

    /* ----- post‑condition checks ----- */

    /* 1. validity predicate */
    assert(aws_byte_buf_is_valid(&buf));

    /* 2. length / capacity invariants */
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);
    if (len > 0) {
        assert(buf.buffer == bytes);
    } else {
        assert(buf.buffer == NULL);
    }

    /* 3. frame condition – input memory must be unchanged */
    if (len > 0) {
        assert(memcmp(bytes, bytes_copy, len) == 0);
    }

    /* clean up */
    free(bytes);
    free(bytes_copy);
    return 0;
}
