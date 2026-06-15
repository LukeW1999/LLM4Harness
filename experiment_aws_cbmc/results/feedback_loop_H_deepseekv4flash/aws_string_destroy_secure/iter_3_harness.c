#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 10

void aws_string_destroy_secure_harness() {
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);
    *(size_t *)&str->len = len;

    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);
    *(struct aws_allocator **)&str->allocator = allocator;

    for (size_t i = 0; i < len; ++i) {
        str->bytes[i] = nondet_uint8_t();
    }

    __CPROVER_assume(aws_string_is_valid(str));

    aws_string_destroy_secure(str);

    __CPROVER_assert(1, "Function called with non-null allocator");

    struct aws_string *null_str = NULL;
    aws_string_destroy_secure(null_str);
    __CPROVER_assert(1, "Function called with null pointer");
}
