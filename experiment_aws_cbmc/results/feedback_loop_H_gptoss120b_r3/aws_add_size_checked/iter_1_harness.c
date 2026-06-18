#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_u64_checked_harness(void) {
    /* nondeterministic inputs */
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();

    /* output location with saved old value */
    uint64_t r_val = nondet_uint64_t();
    uint64_t old_r = r_val;
    uint64_t *r = &r_val;

    /* call the function */
    int result = aws_add_u64_checked(a, b, r);

    /* result must be one of the defined outcomes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* on success the sum is stored */
        assert(*r == a + b);
    } else {
        /* on overflow the output is unchanged */
        assert(*r == old_r);
    }
}

void aws_add_u32_checked_harness(void) {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();

    uint32_t r_val = nondet_uint32_t();
    uint32_t old_r = r_val;
    uint32_t *r = &r_val;

    int result = aws_add_u32_checked(a, b, r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(*r == old_r);
    }
}

void aws_add_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t r_val = nondet_size_t();
    size_t old_r = r_val;
    size_t *r = &r_val;

    int result = aws_add_size_checked(a, b, r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(*r == old_r);
    }
}
