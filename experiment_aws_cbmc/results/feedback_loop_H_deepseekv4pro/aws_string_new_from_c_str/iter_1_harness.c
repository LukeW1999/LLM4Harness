#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stddef.h>

#define MAX_STRING_LEN 16

void aws_string_new_from_c_str_harness() {
    /* 1. Declare and bound inputs */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* c_str is a valid null-terminated C string with bounded length */
    const char *c_str;
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(aws_c_string_is_valid(c_str));
    __CPROVER_assume(strlen(c_str) <= MAX_STRING_LEN);

    /* 2. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 3. Assert postconditions for both success and failure paths */
    if (result != NULL) {
        /* Success: returned a valid aws_string */
        assert(aws_string_is_valid(result));

        /* Allocator stored correctly */
        assert(result->allocator == allocator);

        /* Length matches the input C string */
        assert(result->len == strlen(c_str));

        /* Data bytes match the original C string (excluding null terminator) */
        assert(memcmp(result->bytes, c_str, result->len) == 0);

        /* The string is null-terminated */
        assert(result->bytes[result->len] == '\0');
    } else {
        /* Failure: allocation failed, result is NULL */
        assert(result == NULL);
    }

    /* 4. Inputs remain unchanged (c_str is const, allocator is not modified) */
    /* No mutable input structures to check for unchanged fields */

    /* 5. Validity invariants: already checked for result if non-NULL */
}
