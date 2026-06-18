#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_destroy_secure_harness(void) {
    struct aws_string *str;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        uint8_t data[MAX_STRING_LEN];
        for (size_t i = 0; i < len; ++i) {
            data[i] = nondet_uint8_t();
        }

        struct aws_allocator *allocator = aws_default_allocator();
        str = aws_string_new_from_array(allocator, data, len);
        __CPROVER_assume(str != NULL);

        aws_string_destroy_secure(str);
    }
}
