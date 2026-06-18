#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUF 64

void aws_ptr_eq_harness(void) {
    /* Allocate a nondeterministic buffer and fill it with nondet data */
    size_t buf_len = nondet_uint();
    __CPROVER_assume(buf_len <= MAX_BUF);
    uint8_t *buf = (uint8_t *)malloc(buf_len);
    __CPROVER_assume(buf != NULL);
    for (size_t i = 0; i < buf_len; ++i) {
        buf[i] = nondet_uint8();
    }

    /* Make a copy of the buffer to check for modifications after the call */
    uint8_t *buf_snapshot = (uint8_t *)malloc(buf_len);
    __CPROVER_assume(buf_snapshot != NULL);
    for (size_t i = 0; i < buf_len; ++i) {
        buf_snapshot[i] = buf[i];
    }

    /* Choose nondeterministic pointers a and b.
       They may be NULL, point inside the buffer, or be arbitrary values. */
    const void *a;
    const void *b;

    bool a_is_null = nondet_bool();
    bool b_is_null = nondet_bool();

    if (a_is_null) {
        a = NULL;
    } else {
        /* a points somewhere inside the buffer */
        size_t a_offset = nondet_uint();
        __CPROVER_assume(a_offset < buf_len);
        a = (const void *)(buf + a_offset);
    }

    if (b_is_null) {
        b = NULL;
    } else {
        /* b points somewhere inside the buffer */
        size_t b_offset = nondet_uint();
        __CPROVER_assume(b_offset < buf_len);
        b = (const void *)(buf + b_offset);
    }

    /* Call the function under verification */
    bool result = aws_ptr_eq(a, b);

    /* Postcondition 1: return value must reflect pointer equality */
    assert(result == (a == b));

    /* Postcondition 2: the buffer must remain unchanged (frame condition) */
    for (size_t i = 0; i < buf_len; ++i) {
        assert(buf[i] == buf_snapshot[i]);
    }

    /* Clean up */
    free(buf);
    free(buf_snapshot);
    return 0;
}
