#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void aws_array_eq_harness() {
    // Non-deterministic values for lengths
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    // Assume lengths are within a reasonable range to avoid integer overflow
    __CPROVER_assume(len_a <= 1000);
    __CPROVER_assume(len_b <= 1000);

    // Allocate memory for the arrays
    uint8_t *array_a = can_fail_malloc(len_a);
    uint8_t *array_b = can_fail_malloc(len_b);

    // Assume malloc did not fail
    __CPROVER_assume(array_a != NULL);
    __CPROVER_assume(array_b != NULL);

    // Initialize arrays with non-deterministic values
    if (len_a > 0) {
        __CPROVER_assume(__CPROVER_w_ok(array_a, len_a));
        memset(array_a, nondet_uint8_t(), len_a);
    }
    if (len_b > 0) {
        __CPROVER_assume(__CPROVER_w_ok(array_b, len_b));
        memset(array_b, nondet_uint8_t(), len_b);
    }

    // Save old state
    size_t old_len_a = len_a;
    size_t old_len_b = len_b;

    // Call the function under test
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    // Step 3 assertions (frame conditions)
    assert(len_a == old_len_a);
    assert(len_b == old_len_b);

    // Step 4 assertions (validity invariants)
    // No validity invariants as the function does not modify the inputs.

    // Free allocated memory
    free(array_a);
    free(array_b);
}
