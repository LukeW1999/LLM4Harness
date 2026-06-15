#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_string_new_from_c_str_harness() {
    /* Constants */
    const size_t MAX_STRING_LEN = 10;
    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);

    /* Non-deterministic inputs */
    struct aws_allocator *allocator = can_fail_allocator();
    char *c_str = malloc(sizeof(char) * (len + 1));
    __CPROVER_assume(c_str != NULL);
    /* Fill with non-deterministic bytes, ensuring null-termination */
    for (size_t i = 0; i < len; i++) {
        c_str[i] = nondet_char();
    }
    c_str[len] = '\0';

    /* Preconditions */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* Call function */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Postconditions */
    if (result != NULL) {
        assert(aws_string_is_valid(result));
        assert(result->len == len);
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);
        assert(result->allocator == allocator);
    }
}
