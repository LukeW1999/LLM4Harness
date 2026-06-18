#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness() {
    /* 1. Declare a nondeterministic string pointer and assume it is valid */
    struct aws_string *str;
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save the old state of the string (fields that are observable) */
    struct aws_string old = *str;

    /* 3. Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Post‑conditions: the returned pointer must be the bytes field */
    assert(result == str->bytes);

    /* 5. Unchanged fields (the function does not modify the string) */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);
    assert(str->bytes == old.bytes);

    /* 6. The string must remain valid after the call */
    assert(aws_string_is_valid(str));
}
