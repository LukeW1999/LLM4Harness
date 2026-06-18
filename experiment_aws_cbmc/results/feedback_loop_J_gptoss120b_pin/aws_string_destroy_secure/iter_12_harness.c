#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_SIZE 256

void aws_string_destroy_secure_harness() {
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;

    uint8_t buffer[sizeof(struct aws_string) + MAX_STRING_SIZE];

    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);

        str = (struct aws_string *)buffer;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;

        uint8_t *bytes = (uint8_t *)(str + 1);
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    aws_string_destroy_secure(str);
}
