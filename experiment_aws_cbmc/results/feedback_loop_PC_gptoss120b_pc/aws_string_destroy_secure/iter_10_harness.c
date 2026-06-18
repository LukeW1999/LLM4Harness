#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;
    bool is_null = nondet_bool();

    if (is_null) {
        str = NULL;
    } else {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);

        uint8_t buffer[sizeof(struct aws_string) + MAX_STRING_LEN + 1];
        str = (struct aws_string *)buffer;

        str->allocator = allocator;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)str->bytes)[len] = '\0';
    }

    aws_string_destroy_secure(str);
}
