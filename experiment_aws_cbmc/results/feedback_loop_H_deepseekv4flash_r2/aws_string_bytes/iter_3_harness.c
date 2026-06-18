#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness() {
    /* Non-deterministic parameters */
    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);

    /* Allocate memory for the string (struct + bytes + null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);

    /* Initialize fields */
    str->allocator = aws_default_allocator();
    str->len = len;

    /* Fill bytes with nondet values and guarantee null terminator */
    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = '\0';

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old state */
    struct aws_string old_str = *str;

    /* Call function */
    const uint8_t *result = aws_string_bytes(str);

    /* Postconditions */
    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
}
