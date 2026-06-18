#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* AWS operation result codes */
#ifndef AWS_OP_SUCCESS
#   define AWS_OP_SUCCESS 0
#endif
#ifndef AWS_OP_ERR
#   define AWS_OP_ERR (-1)
#endif

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Non‑deterministic input, bounded */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);

    /* 2. Allocate output storage and remember its old value */
    size_t result_storage = nondet_size_t();
    __CPROVER_assume(result_storage <= MAX_BUFFER_SIZE);
    size_t old_result = result_storage;
    size_t *result = &result_storage;

    /* 3. Call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 4. Post‑condition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert((*result & (*result - 1)) == 0);
        /* result must be >= n */
        assert(*result >= n);
        /* result must be the smallest such power of two */
        if (n > 0) {
            assert((*result >> 1) < n);
        } else {
            /* special case n == 0 yields result == 1 */
            assert(*result == 1);
        }
    } else {
        /* Failure path: function returns AWS_OP_ERR and does not modify *result */
        assert(rc == AWS_OP_ERR);
        assert(*result == old_result);
    }

    /* 5. Input n is unchanged (trivially true for by‑value parameters) */
    assert(n == n);
}
