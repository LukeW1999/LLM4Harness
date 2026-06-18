#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

/* CBMC nondeterministic size_t */
extern size_t nondet_size_t(void);

void aws_mul_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate result storage */
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);

    /* guard variables to check frame conditions */
    size_t guard_before = nondet_size_t();
    size_t guard_after  = nondet_size_t();
    size_t guard_before_snapshot = guard_before;
    size_t guard_after_snapshot  = guard_after;

    /* invoke the function under test */
    int ret = aws_mul_size_checked(a, b, r);

    /* post‑condition: return‑code correctness */
    if (ret == AWS_OP_SUCCESS) {
        /* no overflow: the product must be exact */
        assert(*r == a * b);
    } else {
        /* overflow case: at least one operand is non‑zero and the product would exceed SIZE_MAX */
        assert(!(a == 0 || b == 0 || a <= SIZE_MAX / b));
    }

    /* frame condition: memory outside the contract is unchanged */
    assert(guard_before == guard_before_snapshot);
    assert(guard_after  == guard_after_snapshot);

    free(r);
    return 0;
}
