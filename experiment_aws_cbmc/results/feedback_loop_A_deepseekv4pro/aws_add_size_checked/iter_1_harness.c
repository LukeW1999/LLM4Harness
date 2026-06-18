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
    uint64_t *r = malloc(sizeof(*r));

    /* Precondition: r must be writable */
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    int result = aws_add_u64_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        /* Success: *r must equal a + b */
        assert(*r == a + b);
    } else {
        /* Failure: result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
        /* On failure, *r is not modified (implementation does not write to *r) */
    }

    free(r);
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t *r = malloc(sizeof(*r));

    /* Precondition: r must be writable */
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    int result = aws_add_u32_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        /* Success: *r must equal a + b */
        assert(*r == a + b);
    } else {
        /* Failure: result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
        /* On failure, *r is not modified */
    }

    free(r);
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r = malloc(sizeof(*r));

    /* Precondition: r must be writable */
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        /* Success: *r must equal a + b */
        assert(*r == a + b);
    } else {
        /* Failure: result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
        /* On failure, *r is not modified */
    }

    free(r);
}
