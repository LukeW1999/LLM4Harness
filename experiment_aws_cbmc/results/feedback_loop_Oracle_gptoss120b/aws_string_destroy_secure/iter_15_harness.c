#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN);

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_string *str = nondet_bool()
        ? NULL
        : (struct aws_string *)allocator->mem_acquire(allocator, sizeof(struct aws_string) + len);
    if (str) {
        str->allocator = allocator;
        str->len = len;
        for (size_t j = 0; j < len; ++j) {
            ((uint8_t *)str->bytes)[j] = nondet_uint8_t();
        }
    }

    size_t old_len = 0;
    if (str) {
        old_len = str->len;
        if (old_len > 0) {
            size_t i = nondet_size_t();
            __CPROVER_assume(i < old_len);
            (void)i;
        }
    }

    __CPROVER_assume(str == NULL || aws_string_is_valid(str));

    aws_string_destroy_secure(str);

    if (str) {
        if (old_len > 0) {
            size_t i = nondet_size_t();
            __CPROVER_assume(i < old_len);
            assert(((uint8_t *)str->bytes)[i] == 0);
        }
        assert(str->len == old_len);
    } else {
        assert(str == NULL);
    }
}
