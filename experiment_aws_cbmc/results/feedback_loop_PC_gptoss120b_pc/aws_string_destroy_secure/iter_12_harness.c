#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str = NULL;
    bool is_null = nondet_bool();

    if (!is_null) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);

        uint8_t *data = (uint8_t *)malloc(len);
        __CPROVER_assume(data != NULL);
        for (size_t i = 0; i < len; ++i) {
            data[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(allocator, data, len);
    }

    aws_string_destroy_secure(str);
}
