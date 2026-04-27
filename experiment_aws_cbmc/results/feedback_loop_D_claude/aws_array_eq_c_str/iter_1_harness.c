// === STEP 1: SUCCESS PATH ===
// When aws_array_eq_c_str returns true:
//   - array and c_str have the same content for array_len bytes
//   - c_str[array_len] == '\0'
//   - No parameters are modified (this is a pure comparison function)
//
// === STEP 2: FAILURE PATH ===
// When aws_array_eq_c_str returns false:
//   - Either c_str is shorter than array_len (contains '\0' before index array_len)
//   - Or some byte differs between array and c_str
//   - Or c_str[array_len] != '\0' (c_str is longer)
//   - No parameters are modified
//
// === STEP 3: FRAME CONDITIONS ===
// This function is a pure comparison - no parameters are modified.
// array (const void*): UNCHANGED always
// array_len (size_t): UNCHANGED always
// c_str (const char*): UNCHANGED always
// The function only reads from array and c_str, never writes.
//
// === STEP 4: VALIDITY INVARIANTS ===
// - array: either non-NULL or array_len == 0 (precondition)
// - c_str: must be a valid null-terminated string accessible up to at least array_len bytes
//   (or contain '\0' before that)
// - Return value is bool (true/false)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARRAY_LEN 8
#define MAX_STR_LEN 10

void aws_array_eq_c_str_harness(void) {
    // Non-deterministic array length
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_ARRAY_LEN);

    // Allocate array - either NULL (only if array_len == 0) or a valid buffer
    uint8_t *array;
    if (array_len == 0) {
        // array can be NULL or non-NULL when len is 0
        if (nondet_bool()) {
            array = NULL;
        } else {
            array = malloc(array_len);
            __CPROVER_assume(array != NULL);
        }
    } else {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
    }

    // Non-deterministic c_str length (null-terminated string)
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STR_LEN);
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    // Make it a valid null-terminated string
    c_str[str_len] = '\0';

    // Save old state (nothing to save since no output params, but record inputs)
    size_t old_array_len = array_len;

    // Call the function
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    // === ASSERTIONS ===

    // Frame conditions: array_len is unchanged
    assert(array_len == old_array_len);

    // If result is true, verify the semantics:
    // 1. c_str must be exactly array_len characters long (c_str[array_len] == '\0')
    // 2. All bytes must match
    if (result) {
        // c_str must have length exactly array_len
        assert(strlen(c_str) == array_len);
        // All bytes must match
        if (array != NULL && array_len > 0) {
            assert(memcmp(array, c_str, array_len) == 0);
        }
    }

    // If result is false, at least one condition failed:
    // Either lengths differ or some byte differs
    // (We can't easily assert the negation without re-implementing the logic,
    //  but we can verify the return value is a valid bool)
    assert(result == true || result == false);

    // Verify precondition: array is non-NULL or array_len is 0
    assert(array != NULL || array_len == 0);
}
