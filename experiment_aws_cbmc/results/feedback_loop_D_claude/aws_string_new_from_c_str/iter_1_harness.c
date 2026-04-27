// === STEP 1: SUCCESS PATH ===
// When aws_string_new_from_c_str returns a non-NULL pointer:
//   - Returns a newly allocated aws_string with:
//     - str->len == strlen(c_str)
//     - str->bytes contains the same bytes as c_str
//     - str->allocator == allocator
//   - The returned string is valid (aws_string_is_valid returns true)
//
// === STEP 2: FAILURE PATH ===
// When aws_string_new_from_c_str returns NULL:
//   - Allocation failed
//   - No output to check (returns NULL)
//
// === STEP 3: FRAME CONDITIONS ===
// The function takes:
//   allocator (struct aws_allocator *): used for allocation, not modified by harness
//   c_str (const char *): read-only input string
// Return value: struct aws_string * (newly allocated or NULL)
//   - result->allocator: SET to allocator
//   - result->len: SET to strlen(c_str)
//   - result->bytes: SET to copy of c_str bytes
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - If result != NULL: aws_string_is_valid(result) must hold
//   - result->len == strlen(c_str) on success
//   - result->bytes[result->len] == '\0' (null terminated)

#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

void aws_string_new_from_c_str_harness(void) {
    /* Non-deterministic allocator */
    struct aws_allocator *allocator = can_fail_allocator();

    /* Create a bounded c_str for the proof to be tractable */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    
    /* Allocate a c_str of length len + 1 (for null terminator) */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    
    /* Ensure it's null-terminated */
    c_str[len] = '\0';
    
    /* Save the length before calling */
    size_t expected_len = strlen(c_str);
    /* Since c_str[len] = '\0' and bytes before may be anything,
       strlen could be <= len. We need to ensure strlen == len for
       a clean proof, so assume no null bytes before position len */
    __CPROVER_assume(expected_len == len);

    /* Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        /* SUCCESS PATH assertions */
        
        /* The string must be valid */
        assert(aws_string_is_valid(result));
        
        /* Length must match strlen(c_str) */
        assert(result->len == expected_len);
        
        /* Allocator must match */
        assert(result->allocator == allocator);
        
        /* The bytes must be null-terminated */
        assert(result->bytes[result->len] == '\0');
        
        /* The bytes must match the input c_str */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, expected_len);
    }
    /* FAILURE PATH: result == NULL, nothing to assert */
}
