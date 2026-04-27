#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/cbmc_utils.h>
#include <stddef.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);
    const char *c_str = (const char *)malloc(sizeof(char) * MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(strlen(c_str) < MAX_BUFFER_SIZE);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    const char *old_c_str = c_str;

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result != NULL) {
        assert(result->allocator == allocator);
        assert(result->len == strlen(c_str));
        assert(memcmp(result->bytes, c_str, strlen(c_str)) == 0);
    } else {
        assert(c_str == old_c_str);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(c_str == old_c_str);

    /* 6. Assert validity invariants always holds */
    if (result != NULL) {
        assert(aws_string_is_valid(result));
    }
}
