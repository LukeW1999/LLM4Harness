#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;
    bool have_str = nondet_bool();

    if (have_str) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t buffer[MAX_STRING_LEN];
        for (size_t i = 0; i < len; ++i) {
            buffer[i] = nondet_uint8_t();
        }

        struct aws_allocator *allocator = aws_default_allocator();
        str = aws_string_new_from_array(allocator, buffer, len);
        __CPROVER_assume(str != NULL);
    }

    aws_string_destroy(str);
}
