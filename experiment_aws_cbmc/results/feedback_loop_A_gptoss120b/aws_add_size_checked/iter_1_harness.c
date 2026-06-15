#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "aws/common/math.h"
#include "proof_helpers/make_common_data_structures.h"

/* Harness for aws_add_u64_checked */
void aws_add_u64_checked_harness(void) {
    /* nondeterministic inputs */
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    /* output location */
    uint64_t r = nondet_uint64_t();
    uint64_t old_r = r;               /* save old value for failure case */

    int result = aws_add_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        /* success: result must be the sum */
        assert(r == a + b);
    } else {
        /* failure: output must be unchanged */
        assert(r == old_r);
    }

    /* result must be one of the defined return codes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}

/* Harness for aws_add_u32_checked */
void aws_add_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t r = nondet_uint32_t();
    uint32_t old_r = r;

    int result = aws_add_u32_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}

/* Harness for aws_add_size_checked */
void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t r = nondet_size_t();
    size_t old_r = r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);
}
