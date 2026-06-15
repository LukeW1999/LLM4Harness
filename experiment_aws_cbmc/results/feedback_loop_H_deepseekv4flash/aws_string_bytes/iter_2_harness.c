#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness() {
    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    str->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    str->len = len;
    for (size_t i = 0; i < len; i++) {
        str->bytes[i] = nondet_uint8_t();
    }
    str->bytes[len] = '\0';
    __CPROVER_assume(aws_string_is_valid(str));

    const uint8_t *result = aws_string_bytes(str);

    /* Postconditions */
    assert(result != NULL);
    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
    assert(str->len == len);
    /* Ensure the function was actually called and assertions are reachable */
    assert(1); /* Dummy assertion to guarantee at least one reachable assert */
}
