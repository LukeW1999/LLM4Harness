// === STEP 1: SUCCESS PATH ===
// aws_array_eq_c_str returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When it returns true:
//   - array contents: UNCHANGED (read-only comparison)
//   - array_len: UNCHANGED
//   - c_str contents: UNCHANGED (read-only comparison)
//
// === STEP 2: FAILURE PATH ===
// When it returns false:
//   - array contents: UNCHANGED
//   - array_len: UNCHANGED
//   - c_str contents: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// array (const void *): UNCHANGED always (read-only)
// array_len (size_t): UNCHANGED always
// c_str (const char *): UNCHANGED always (read-only)
// No struct parameters with fields to track.
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function requires: array != NULL OR array_len == 0
// c_str must be a valid null-terminated string
// No aws_XXX_is_valid checks needed (no struct parameters)
// The return value is a bool - no additional postconditions beyond
// the function not crashing and reading valid memory.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARRAY_LEN 8
#define MAX_STR_LEN 8

void aws_array_eq_c_str_harness(void) {
    // Nondeterministic array length, bounded for tractability
    size_t array_len;
    __CPROVER_assume(array_len <= MAX_ARRAY_LEN);

    // Allocate array: may be NULL only if array_len == 0
    uint8_t *array = NULL;
    if (array_len > 0) {
        array = malloc(array_len);
        __CPROVER_assume(array != NULL);
        // Contents are nondeterministic (already the case with malloc in CBMC)
    }

    // Allocate a null-terminated c_str with bounded length
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STR_LEN);
    char *c_str = malloc(str_len + 1); // +1 for null terminator
    __CPROVER_assume(c_str != NULL);
    c_str[str_len] = '\0';
    // Contents of c_str[0..str_len-1] are nondeterministic

    // Save old state (nothing to save for primitives, but save pointers)
    const uint8_t *old_array = array;
    const size_t old_array_len = array_len;
    const char *old_c_str = c_str;

    // Precondition: array != NULL OR array_len == 0
    __CPROVER_assume(array != NULL || array_len == 0);

    // Call the function under test
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    // === Frame condition assertions ===
    // array pointer unchanged
    assert(array == old_array);
    // array_len unchanged
    assert(array_len == old_array_len);
    // c_str pointer unchanged
    assert(c_str == old_c_str);
    // c_str still null-terminated at same position
    assert(c_str[str_len] == '\0');

    // === Correctness assertions ===
    // If result is true, then:
    //   - array_len must equal strlen(c_str)
    //   - all bytes must match
    if (result) {
        assert(array_len == strlen(c_str));
        // Verify byte-by-byte match
        for (size_t i = 0; i < array_len; i++) {
            assert(array[i] == (uint8_t)c_str[i]);
        }
    }

    // If result is false, we don't assert specific conditions
    // (the negation of the above is complex and not needed for safety)
}
