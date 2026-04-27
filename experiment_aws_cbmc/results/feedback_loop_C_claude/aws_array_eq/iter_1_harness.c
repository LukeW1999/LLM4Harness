// === STEP 1: SUCCESS PATH ===
// aws_array_eq returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When aws_array_eq returns true:
//   - len_a == len_b AND (len_a == 0 OR memcmp(array_a, array_b, len_a) == 0)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_eq returns false:
//   - len_a != len_b OR memcmp(array_a, array_b, len_a) != 0
//   - Neither array is modified
//
// === STEP 3: FRAME CONDITIONS ===
// array_a (uint8_t array):
//   - contents: UNCHANGED always (function only reads)
// array_b (uint8_t array):
//   - contents: UNCHANGED always (function only reads)
// len_a: UNCHANGED always (passed by value)
// len_b: UNCHANGED always (passed by value)
//
// === STEP 4: VALIDITY INVARIANTS ===
// - array_a must be readable for len_a bytes (or len_a == 0)
// - array_b must be readable for len_b bytes (or len_b == 0)
// - The function is pure (no side effects on inputs)
// - If result is true, then len_a == len_b
// - If len_a != len_b, result must be false
// - If len_a == 0 && len_b == 0, result must be true

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARRAY_SIZE 8

void aws_array_eq_harness(void) {
    // Nondeterministic lengths
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    // Bound the sizes for tractability
    __CPROVER_assume(len_a <= MAX_ARRAY_SIZE);
    __CPROVER_assume(len_b <= MAX_ARRAY_SIZE);

    // Allocate arrays (or NULL if length is 0)
    uint8_t *array_a = NULL;
    uint8_t *array_b = NULL;

    if (len_a > 0) {
        array_a = malloc(len_a);
        __CPROVER_assume(array_a != NULL);
    }

    if (len_b > 0) {
        array_b = malloc(len_b);
        __CPROVER_assume(array_b != NULL);
    }

    // Save copies of the arrays before the call to check frame conditions
    uint8_t *array_a_copy = NULL;
    uint8_t *array_b_copy = NULL;

    if (len_a > 0) {
        array_a_copy = malloc(len_a);
        __CPROVER_assume(array_a_copy != NULL);
        memcpy(array_a_copy, array_a, len_a);
    }

    if (len_b > 0) {
        array_b_copy = malloc(len_b);
        __CPROVER_assume(array_b_copy != NULL);
        memcpy(array_b_copy, array_b, len_b);
    }

    // Call the function under test
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    // === ASSERTIONS ===

    // Frame condition: arrays are not modified
    if (len_a > 0) {
        assert(memcmp(array_a, array_a_copy, len_a) == 0);
    }
    if (len_b > 0) {
        assert(memcmp(array_b, array_b_copy, len_b) == 0);
    }

    // Correctness: if lengths differ, result must be false
    if (len_a != len_b) {
        assert(result == false);
    }

    // Correctness: if both lengths are 0, result must be true
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    // Correctness: if result is true, lengths must be equal
    if (result == true) {
        assert(len_a == len_b);
    }

    // Correctness: if result is true and len_a > 0, contents must match
    if (result == true && len_a > 0) {
        assert(memcmp(array_a, array_b, len_a) == 0);
    }

    // Correctness: if lengths are equal and contents match, result must be true
    if (len_a == len_b && len_a > 0) {
        if (memcmp(array_a, array_b, len_a) == 0) {
            assert(result == true);
        } else {
            assert(result == false);
        }
    }
}
