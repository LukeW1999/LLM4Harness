#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    /* Use the default allocator; it never fails but the allocation itself may fail */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Prepare a bounded C string */
    char c_str[MAX_BUFFER_SIZE];
    size_t null_pos;
    __CPROVER_assume(null_pos < MAX_BUFFER_SIZE);

    /* Force exactly one null terminator at null_pos */
    for (size_t i = 0; i < null_pos; i++) {
        c_str[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[null_pos] = '\0';
    /* Bytes after null_pos are unconstrained; we fill them nondeterministically */
    for (size_t i = null_pos + 1; i < MAX_BUFFER_SIZE; i++) {
        c_str[i] = (char)nondet_uint8_t();
    }

    /* Save the entire original array to verify immutability */
    char old_c_str[MAX_BUFFER_SIZE];
    memcpy(old_c_str, c_str, MAX_BUFFER_SIZE);

    size_t old_len = null_pos; /* guaranteed by the assumptions above */

    /* Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, (const char *)c_str);

    /* Postconditions: the original C string must never be modified */
    assert(memcmp(c_str, old_c_str, MAX_BUFFER_SIZE) == 0);
    assert(strlen(c_str) == old_len);

    /* Handle both return paths */
    if (result != NULL) {
        /* Success: a valid string was allocated */

        /* The returned string satisfies its validity invariant */
        assert(aws_string_is_valid(result));

        /* The length matches the original C string length */
        assert(result->len == old_len);

        /* The allocator stored in the string is the one we passed in */
        assert(result->allocator == allocator);

        /* The data bytes exactly match the original C string (excluding the null terminator) */
        const uint8_t *bytes = aws_string_bytes(result);
        for (size_t i = 0; i < old_len; i++) {
            assert(bytes[i] == (uint8_t)old_c_str[i]);
        }
        /* The null terminator is present immediately after the data (verified by aws_string_is_valid) */
    } else {
        /* Failure: return is NULL, nothing was allocated, and the input is untouched (already covered) */
    }
}
