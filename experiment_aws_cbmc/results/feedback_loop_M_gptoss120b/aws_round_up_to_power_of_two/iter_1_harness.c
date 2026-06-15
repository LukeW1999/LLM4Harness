#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>

/* CBMC constants defined in the Makefile */
#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE (1ULL << 20) /* fallback if not provided */
#endif

void aws_round_up_to_power_of_two_harness(void) {
    /* 1. Non‑deterministic input, bounded to keep CBMC tractable */
    size_t n = nondet_size_t();
    __CPROVER_assume(n <= MAX_BUFFER_SIZE);

    /* 2. Allocate storage for the output pointer and save its old value */
    size_t result_storage = nondet_size_t();
    size_t *result = &result_storage;
    size_t old_result = result_storage;

    /* 3. Call the function under test */
    int rc = aws_round_up_to_power_of_two(n, result);

    /* 4. Post‑condition checks */
    if (rc == AWS_OP_SUCCESS) {
        /* The function must write a power‑of‑two value >= n */
        assert(*result != 0);
        assert(aws_is_power_of_two(*result));
        assert(*result >= n);
        /* Special case when n == 0, result must be 1 */
        if (n == 0) {
            assert(*result == 1);
        }
    } else {
        /* On error the output must be left unchanged */
        assert(*result == old_result);
    }

    /* 5. Unchanged inputs */
    assert(n == n); /* n is passed by value, trivially unchanged */

    /* 6. General validity invariants (none specific for this function) */
    /* No additional structure validity to check */
}
