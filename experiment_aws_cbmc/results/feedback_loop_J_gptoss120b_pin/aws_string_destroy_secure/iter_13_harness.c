#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_SIZE 256

void aws_string_destroy_secure_harness() {
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;

    struct {
        struct aws_string s;
        uint8_t data[MAX_STRING_SIZE];
    } wrapper;

    if (!is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);

        str = &wrapper.s;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;

        uint8_t *bytes = wrapper.data;
        for (size_t i = 0; i < len; ++i) {
            bytes[i] = nondet_uint8_t();
        }
    }

    aws_string_destroy_secure(str);
}
