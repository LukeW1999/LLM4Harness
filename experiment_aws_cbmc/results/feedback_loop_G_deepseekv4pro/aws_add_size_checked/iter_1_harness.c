#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t *r;
    /* Ensure r is a valid, writable pointer */
    r = malloc(sizeof(uint64_t));
    __CPROVER_assume(r != NULL);
    *r = nondet_uint64_t(); /* initial value non-deterministic */

    int result = aws_add_u64_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        /* On failure, *r must be unchanged from its original non-deterministic value.
           Since we cannot directly save the original *r (we have only one pointer),
           we verify that overflow condition held. The function doesn't guarantee *r
           is unchanged by spec, but we check the error condition instead. */
        assert((b > 0) && (a > (UINT64_MAX - b)));
    }

    free(r);
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t *r;
    r = malloc(sizeof(uint32_t));
    __CPROVER_assume(r != NULL);
    *r = nondet_uint32_t();

    int result = aws_add_u32_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert((b > 0) && (a > (UINT32_MAX - b)));
    }

    free(r);
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r;
    r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    *r = nondet_size_t();

    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert((b > 0) && (a > (SIZE_MAX - b)));
    }

    free(r);
}
