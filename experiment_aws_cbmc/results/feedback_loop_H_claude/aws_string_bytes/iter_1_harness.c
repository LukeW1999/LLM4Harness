#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_bytes_harness() {
    /* 1. Declare and set up a valid aws_string */
    /* We need a valid aws_string pointer. We'll allocate one with nondet length. */
    size_t len;
    __CPROVER_assume(len <= 10); /* bound the length for tractability */

    /* Allocate memory for the aws_string header plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Initialize the fields - cast away const for initialization */
    *(size_t *)&str->len = len;
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* 2. Save old state */
    const struct aws_string *old_str = str;
    size_t old_len = str->len;
    struct aws_allocator *old_allocator = str->allocator;
    const uint8_t *old_bytes_ptr = str->bytes;

    /* 3. Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* 4. Assert postconditions */
    /* The function returns str->bytes, which is the address of the bytes field */
    assert(result == str->bytes);
    assert(result == old_bytes_ptr);

    /* 5. Assert fields that must NOT change */
    assert(str->len == old_len);
    assert(str->allocator == old_allocator);
    assert(str == old_str);

    /* 6. Assert validity invariant still holds */
    assert(aws_string_is_valid(str));

    /* 7. Assert the result is non-null (bytes field is always valid for a valid string) */
    assert(result != NULL);
}
