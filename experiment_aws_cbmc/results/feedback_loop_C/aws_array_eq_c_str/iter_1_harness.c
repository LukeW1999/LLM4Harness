// === STEP 1: SUCCESS PATH ===
// When aws_array_eq_c_str returns true:
//   - No changes to any parameters as it is a comparison function.

// === STEP 2: FAILURE PATH ===
// When aws_array_eq_c_str returns false:
//   - No changes to any parameters as it is a comparison function.

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   param1 (void *const array):
//     - Content pointed by array: UNCHANGED always
//   param2 (size_t array_len):
//     - Value: UNCHANGED always
//   param3 (const char *const c_str):
//     - Content pointed by c_str: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - No validity invariant checks required as the function does not modify the parameters.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_array_eq_c_str_harness() {
    size_t array_len = nondet_size_t();
    const char *c_str = can_fail_nondet_char_ptr();
    uint8_t *array = can_fail_malloc(array_len);

    // Save old state
    uint8_t *old_array = array;
    size_t old_array_len = array_len;
    const char *old_c_str = c_str;

    // Assume preconditions
    __CPROVER_assume(array || (array_len == 0));
    if (c_str) {
        size_t c_str_len = strlen(c_str);
        __CPROVER_assume(array_len <= c_str_len);
    }

    // Call the function under test
    bool result = aws_array_eq_c_str(array, array_len, c_str);

    // Assert frame conditions
    assert(array == old_array);
    assert(array_len == old_array_len);
    assert(c_str == old_c_str);

    // Assert postconditions
    // Since the function is a comparison, no changes are expected in the parameters.
}
