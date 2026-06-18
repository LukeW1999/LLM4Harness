#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 100

void aws_string_bytes_harness() {
    /* Use a static structure with a fixed‑size buffer for the flexible array member */
    struct aws_string_storage {
        struct aws_string str;
        uint8_t bytes[MAX_STRING_LEN + 1];
    } storage;
    struct aws_string *str = &storage.str;

    /* Non‑deterministic length */
    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);

    /* Initialize the string */
    str->allocator = aws_default_allocator();
    str->len = len;

    /* Fill the bytes array with arbitrary values and guarantee the null terminator */
    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = '\0';

    /* Assume the string is valid according to the library's contract */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save the old state for post‑condition checks */
    struct aws_string old_str = *str;

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Postconditions */
    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
}
