#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>

void aws_mul_size_checked_harness() {
    /* 1. Declare and bound data structures */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_a = a;
    size_t old_b = b;

    /* 3. Call function under test */
    int result = aws_mul_size_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(r == old_a);  // r should not be modified on failure
        assert(r == old_b);  // r should not be modified on failure
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No other fields to assert as the function only modifies the pointed value of r

    /* 6. Assert validity invariants always holds */
    // No additional validity invariants for primitive types
}

void aws_add_size_checked_harness() {
    /* 1. Declare and bound data structures */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_a = a;
    size_t old_b = b;

    /* 3. Call function under test */
    int result = aws_add_size_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_a);  // r should not be modified on failure
        assert(r == old_b);  // r should not be modified on failure
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No other fields to assert as the function only modifies the pointed value of r

    /* 6. Assert validity invariants always holds */
    // No additional validity invariants for primitive types
}

void aws_sub_size_checked_harness() {
    /* 1. Declare and bound data structures */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_a = a;
    size_t old_b = b;

    /* 3. Call function under test */
    int result = aws_sub_size_checked(a, b, r_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a - b);
    } else {
        assert(r == old_a);  // r should not be modified on failure
        assert(r == old_b);  // r should not be modified on failure
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No other fields to assert as the function only modifies the pointed value of r

    /* 6. Assert validity invariants always holds */
    // No additional validity invariants for primitive types
}

void aws_round_up_to_power_of_two_harness() {
    /* 1. Declare and bound data structures */
    size_t n = nondet_size_t();
    size_t result;
    size_t *result_ptr = &result;

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_n = n;

    /* 3. Call function under test */
    int ret = aws_round_up_to_power_of_two(n, result_ptr);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (ret == AWS_OP_SUCCESS) {
        assert(result >= n);
        assert((result & (result - 1)) == 0);  // result is a power of two
    } else {
        assert(n > SIZE_MAX_POWER_OF_TWO);
        assert(result == old_n);  // result should not be modified on failure
    }

    /* 5. Assert fields that must NOT change regardless of result */
    // No other fields to assert as the function only modifies the pointed value of result

    /* 6. Assert validity invariants always holds */
    // No additional validity invariants for primitive types
}
