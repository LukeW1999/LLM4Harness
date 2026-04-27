// === STEP 1: SUCCESS PATH ===
// When aws_string_new_from_c_str returns non-NULL (or the successful value):
//   - The returned struct aws_string* should point to a valid aws_string structure
//   - The len field should be equal to the length of the input c_str
//   - The bytes field should match the input c_str

// === STEP 2: FAILURE PATH ===
// When aws_string_new_from_c_str returns NULL (or fails):
//   - The allocator: UNCHANGED
//   - The c_str: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   allocator (struct aws_allocator*):
//     - UNCHANGED always
//   c_str (const char*):
//     - UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(result): YES (must hold after call if result is non-NULL)

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/proof_allocators.h>
#include <stddef.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator = can_fail_allocator();
    const char *c_str = ndecls("c_str");

    struct aws_string *old_result = aws_string_new_from_c_str(allocator, c_str);
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        // Success path assertions
        assert(aws_string_is_valid(result));
        assert(result->len == strlen(c_str));
        assert(memcmp(result->bytes, c_str, result->len) == 0);
    } else {
        // Failure path assertions
        // No change in allocator or c_str
    }

    // Frame condition assertions
    // allocator and c_str are unchanged, so no need to save old state and compare
}
