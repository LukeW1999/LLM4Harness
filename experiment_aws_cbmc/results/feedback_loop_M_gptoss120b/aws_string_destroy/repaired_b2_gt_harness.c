#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        uint8_t data[MAX_BUFFER_SIZE];
        for (size_t i = 0; i < len; ++i) {
            data[i] = nondet_uint8_t();
        }

        struct aws_allocator *alloc = aws_default_allocator();
        str = aws_string_new_from_array(alloc, data, len);
    }

    __CPROVER_assume(IMPLIES(str != NULL, aws_string_is_valid(str)));

    aws_string_destroy(str);
}
