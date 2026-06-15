#include <assert.h>
#include <stddef.h>
#include "aws/common/common.h"
#include "aws/common/math.h"
#include "proof_helpers/make_common_data_structures.h"

void aws_round_up_to_power_of_two_harness() {
    /* 1. Non‑deterministic inputs */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= SIZE_MAX);

    /* Allocate storage for the output pointer and give it a nondet initial value */
    size_t result_storage = nondet_size_t();
    size_t *result = &result_storage;

    /* Save old state */
    size_t old_n = n;
    size_t old_result = *result;

    /* 2. Call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 3. Force both success and failure branches to be explored */
    if (nondet_bool()) {
        __CPROVER_assume(rc == AWS_OP_SUCCESS);
        /* Success post‑conditions */
        assert((*result != 0) && ((*result & (*result - 1)) == 0));   /* power of two */
        assert(*result >= old_n);                                    /* not smaller */
        if (old_n == 0) {
            assert(*result == 1);
        }
    } else {
        __CPROVER_assume(rc != AWS_OP_SUCCESS);
        /* Failure post‑conditions */
        assert(*result == old_result);
    }

    /* 4. Invariants that must hold regardless of outcome */
    assert(n == old_n);                /* input unchanged */
    assert(result == &result_storage); /* pointer unchanged */
}
