// === STEP 1: SUCCESS PATH ===
// When aws_array_eq returns true:
//   - len_a == len_b AND (len_a == 0 OR memcmp(array_a, array_b, len_a) == 0)
//   - Neither array_a nor array_b are modified
//
// === STEP 2: FAILURE PATH ===
// When aws_array_eq returns false:
//   - Either len_a != len_b, OR memcmp returned non-zero
//   - Neither array_a nor array_b are modified
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
// The function returns bool:
//   - Returns true iff len_a == len_b AND (len_a == 0 OR memcmp == 0)
//   - Returns false otherwise
// Preconditions: arrays must be readable up to their lengths (or length == 0)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MAX_ARRAY_SIZE 8

void aws_array_eq_harness(void) {
    // Nondeterministic lengths
    size_t len_a = nondet_size_t();
    size_t len_b = nondet_size_t();

    // Bound lengths to keep verification tractable
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

    // Call the function under verification
    bool result = aws_array_eq(array_a, len_a, array_b, len_b);

    // === STEP 1 & 2: Verify return value semantics ===
    if (result) {
        // If true: lengths must be equal
        assert(len_a == len_b);
        // If true and non-zero length: contents must match
        if (len_a > 0) {
            assert(memcmp(array_a, array_b, len_a) == 0);
        }
    } else {
        // If false: either lengths differ, or contents differ
        assert(len_a != len_b || (len_a > 0 && memcmp(array_a, array_b, len_a) != 0));
    }

    // === STEP 3: Frame conditions - arrays must be unchanged ===
    if (len_a > 0) {
        assert(memcmp(array_a, array_a_copy, len_a) == 0);
    }

    if (len_b > 0) {
        assert(memcmp(array_b, array_b_copy, len_b) == 0);
    }

    // === Additional correctness check: result must be consistent ===
    // If lengths are equal and zero, result must be true
    if (len_a == 0 && len_b == 0) {
        assert(result == true);
    }

    // If lengths differ, result must be false
    if (len_a != len_b) {
        assert(result == false);
    }
}
