#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ptr_eq_harness(void) {
    /* nondeterministic choice of whether each pointer is NULL */
    bool a_is_null = __CPROVER_nondet_bool();
    bool b_is_null = __CPROVER_nondet_bool();

    const void *a = NULL;
    const void *b = NULL;

    uint8_t *buf_a = NULL;
    uint8_t *buf_b = NULL;

    uint8_t a_byte_snapshot = 0;
    uint8_t b_byte_snapshot = 0;

    /* allocate a single byte for a if it is not NULL and record its value */
    if (!a_is_null) {
        buf_a = malloc(1);
        __CPROVER_assume(buf_a != NULL);
        buf_a[0] = __CPROVER_nondet_uint8_t();
        a_byte_snapshot = buf_a[0];
        a = buf_a;
    }

    /* allocate a single byte for b if it is not NULL and record its value */
    if (!b_is_null) {
        buf_b = malloc(1);
        __CPROVER_assume(buf_b != NULL);
        buf_b[0] = __CPROVER_nondet_uint8_t();
        b_byte_snapshot = buf_b[0];
        b = buf_b;
    }

    /* call the function under test */
    bool result = aws_ptr_eq(a, b);

    /* postcondition 1: return value matches pointer equality */
    assert(result == (a == b));

    /* postcondition 2: memory pointed to by a (if any) is unchanged */
    if (buf_a != NULL) {
        assert(buf_a[0] == a_byte_snapshot);
    }

    /* postcondition 3: memory pointed to by b (if any) is unchanged */
    if (buf_b != NULL) {
        assert(buf_b[0] == b_byte_snapshot);
    }

    /* clean up */
    free(buf_a);
    free(buf_b);

    return 0;
}
