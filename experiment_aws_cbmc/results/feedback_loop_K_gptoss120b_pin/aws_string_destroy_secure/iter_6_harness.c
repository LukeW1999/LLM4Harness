#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_secure_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 1024);

    union {
        struct aws_string s;
        uint8_t raw[sizeof(struct aws_string) + 1024 + 1];
    } storage;

    struct aws_string *str = &storage.s;

    struct aws_allocator *allocator = aws_default_allocator();
    str->allocator = allocator;
    str->len = len;

    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }
    ((uint8_t *)str->bytes)[len] = 0;

    __CPROVER_assume(aws_string_is_valid(str));

    aws_string_destroy_secure(str);
}
