#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_array_eq_harness() {
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();
    uint8_t *array_a = can_fail_malloc(len_a);
    uint8_t *array_b = can_fail_malloc(len_b);

    __CPROVER_assume(array_a || len_a == 0);
    __CPROVER_assume(array_b || len_b == 0);

    if (array_a) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_a, len_a));
    }
    if (array_b) {
        __CPROVER_assume(AWS_MEM_IS_READABLE(array_b, len_b));
    }

    // Ensure that the lengths are such that the arrays can be safely compared
    __CPROVER_assume(len_a <= SIZE_MAX);
    __CPROVER_assume(len_b <= SIZE_MAX);

    // Store old values
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;
    uint8_t *old_array_a = array_a;
    uint8_t *old_array_b = array_b;

    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    // Assertions for frame conditions
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);
    assert(array_a == old_array_a);
    assert(array_b == old_array_b);

    // Assertions for validity invariants
    // No need for validity checks on non-struct parameters
}
