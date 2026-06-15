#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness() {
    struct aws_allocator *allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    struct aws_string *str = make_aws_string(allocator);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    const uint8_t *result = aws_string_bytes(str);

    /* Postconditions */
    assert(result != NULL);
    assert(result == str->bytes);
    assert(aws_string_is_valid(str));
    assert(str->len == strlen((const char *)str->bytes));
}
