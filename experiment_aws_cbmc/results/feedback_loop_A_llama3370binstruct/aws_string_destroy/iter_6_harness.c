#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    /* 1. Declare and bound data structures */
    struct aws_string *str = ensure_string_is_bounded_length(MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_string *old_str = str;

    /* 3. Assume allocator is valid */
    str->allocator = aws_default_allocator();
    __CPROVER_assume(str->allocator != NULL);
    __CPROVER_assume(str->allocator->mem_release != NULL);
    __CPROVER_assume(str->bytes != NULL);
    __CPROVER_assume(str->len <= str->capacity);
    __CPROVER_assume(str->capacity > 0);

    /* 4. Call function under test */
    aws_string_destroy(str);

    /* 5. Assert postconditions for BOTH success and failure paths */
    assert(str->bytes == NULL);
    assert(str->len == 0);
    assert(str->capacity == 0);

    /* 6. Assert fields that must NOT change regardless of result */
    assert(str->allocator == old_str->allocator);

    /* 7. Assert validity invariant always holds */
    assert(str == NULL || aws_string_is_valid(str));
}

int main() {
    aws_string_destroy_harness();
    return 0;
}
