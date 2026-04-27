// === STEP 1: SUCCESS PATH ===
// When aws_string_new_from_c_str succeeds (returns non-NULL):
//   - Returns a newly allocated aws_string with:
//     - result->len == strlen(c_str)
//     - result->bytes contains the same bytes as c_str
//     - result->allocator == allocator
//
// === STEP 2: FAILURE PATH ===
// When aws_string_new_from_c_str returns NULL (allocation failure):
//   - Returns NULL
//   - No output parameters to check (return value only)
//
// === STEP 3: FRAME CONDITIONS ===
// The function takes:
//   - allocator (struct aws_allocator *): used for allocation, not modified
//   - c_str (const char *): read-only input, not modified
// No struct parameters are modified by this function.
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - If result != NULL: aws_string_is_valid(result) must hold
//   - result->len == strlen(c_str) on success
//   - result->allocator == allocator on success

#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

/* Bound on c_str length to keep verification tractable */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 16
#endif

void aws_string_new_from_c_str_harness(void) {
    /* Set up allocator */
    struct aws_allocator *allocator = can_fail_allocator();

    /* Create a bounded, null-terminated c_str */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STRING_LEN);

    /* Allocate a buffer of str_len + 1 bytes (for null terminator) */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Fill with arbitrary bytes, ensure null terminator */
    /* CBMC will treat the bytes as nondet; we just need the null terminator */
    c_str[str_len] = '\0';

    /* Save the length before the call */
    size_t expected_len = strlen(c_str);
    /* Since c_str[str_len] = '\0' and we don't constrain earlier bytes,
     * strlen could be <= str_len. We capture it here. */

    /* Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        /* SUCCESS PATH assertions */

        /* The string must be valid */
        assert(aws_string_is_valid(result));

        /* Length must match strlen of the input */
        assert(result->len == expected_len);

        /* Allocator must match */
        assert(result->allocator == allocator);

        /* The bytes must match the input c_str */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, expected_len);

        /* The string must be null-terminated after the data */
        assert(result->bytes[result->len] == '\0');
    }
    /* FAILURE PATH: result == NULL, nothing else to check */
}
