#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_secure_harness(void) {
    size_t len = __CPROVER_nondet_size_t();
    __CPROVER_assume(len <= 1024);

    uint8_t storage[sizeof(struct aws_string) + 1024 + 1];
    struct aws_string *str = (struct aws_string *)storage;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = (uint8_t)__CPROVER_nondet_unsigned_char();
    }
    ((uint8_t *)str->bytes)[len] = 0;

    __CPROVER_assume(aws_string_is_valid(str));

    aws_string_destroy_secure(str);
}
