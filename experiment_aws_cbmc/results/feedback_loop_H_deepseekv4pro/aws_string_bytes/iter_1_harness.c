#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <assert.h>

void aws_string_bytes_harness() {
    /* 1. Declare and bound data structures */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate memory for the string header and its data bytes (including null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    /* Initialize the string fields */
    str->allocator = aws_default_allocator();
    str->len = len;

    /* Ensure the bytes array is fully readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, len + 1));

    /* Assume the string satisfies the validity invariant */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state (allocator and len) */
    struct aws_allocator *const old_allocator = str->allocator;
    const size_t old_len = str->len;

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */
    /* The returned pointer must be exactly str->bytes */
    assert(result == str->bytes);

    /* The string fields must remain unchanged */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);

    /* The string must still be valid after the call */
    assert(aws_string_is_valid(str));

    /* Free allocated memory (optional, for completeness) */
    free(str);
}
