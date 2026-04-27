#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    /* 1. Declare and bound data structures */
    struct aws_allocator *allocator = aws_default_allocator();
    size_t str_len;
    __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
    char c_str[MAX_BUFFER_SIZE];
    __CPROVER_assume(c_str[str_len] == '\0');

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    char old_c_str[MAX_BUFFER_SIZE];
    memcpy(old_c_str, c_str, sizeof(c_str));

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result != NULL) {
        assert(result->allocator == allocator);
        assert(result->len == strlen(c_str));
        assert(memcmp(result->bytes, c_str, strlen(c_str)) == 0);
    } else {
        assert(memcmp(c_str, old_c_str, sizeof(c_str)) == 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(memcmp(c_str, old_c_str, sizeof(c_str)) == 0);

    /* 6. Assert validity invariants always holds */
    if (result != NULL) {
        assert(aws_string_is_valid(result));
    }
}
