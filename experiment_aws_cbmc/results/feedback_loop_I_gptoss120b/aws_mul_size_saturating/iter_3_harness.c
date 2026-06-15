#include <aws/common/common.h>
#include <aws/common/math.h>
#include <aws/common/memory.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    /* 1. nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Allocate result pointer using the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    size_t *result = (size_t *)aws_mem_acquire(alloc, sizeof(size_t));
    __CPROVER_assume(result != NULL);

    /* 2. Call the function under test */
    int rv = aws_mul_size_saturating(a, b, result);

    /* 3. Post‑conditions */

    /* Validity predicate: result pointer must be non‑null */
    assert(result != NULL);

    /* Return value must be either success or error */
    assert(rv == AWS_OP_SUCCESS || rv == AWS_OP_ERR);

    if (a == 0 || b == 0) {
        /* Zero multiplication case */
        assert(*result == 0);
        assert(rv == AWS_OP_SUCCESS);
    } else if (a > SIZE_MAX / b) {
        /* Overflow case */
        assert(*result == SIZE_MAX);
        assert(rv == AWS_OP_ERR);
    } else {
        /* Normal case */
        assert(*result == a * b);
        assert(rv == AWS_OP_SUCCESS);
    }

    /* 4. Clean up */
    aws_mem_release(alloc, result);
}
