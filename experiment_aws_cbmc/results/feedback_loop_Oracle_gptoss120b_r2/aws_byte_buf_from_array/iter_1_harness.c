/* CBMC harness for aws_byte_buf_from_array */
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* nondeterministic length, bounded to keep verification tractable */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 256);

    /* allocate a writable buffer when len > 0 */
    uint8_t *bytes = NULL;
    uint8_t *bytes_old = NULL;

    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);

        /* initialise with nondeterministic contents */
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }

        /* keep a copy to check frame condition */
        bytes_old = malloc(len);
        __CPROVER_assume(bytes_old != NULL);
        memcpy(bytes_old, bytes, len);
    } else {
        /* when len == 0 the precondition permits any pointer, including NULL */
        bytes = NULL;
    }

    /* call the function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(bytes, len);

    /* ----- post‑condition checks ----- */

    /* 1. validity predicate */
    assert(aws_byte_buf_is_valid(&buf));

    /* 2. length / capacity invariants */
    assert(buf.len == len);
    assert(buf.capacity == len);
    if (len == 0) {
        assert(buf.buffer == NULL);
    } else {
        assert(buf.buffer == bytes);
    }
    assert(buf.allocator == NULL);

    /* 3. frame condition – input memory must be unchanged */
    if (len > 0) {
        for (size_t i = 0; i < len; ++i) {
            assert(bytes[i] == bytes_old[i]);
        }
    }

    /* clean up */
    free(bytes);
    free(bytes_old);
    return 0;
}
