#include <assert.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_LEN 1024

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;

    if (__CPROVER_nondet_bool()) {
        size_t len = __CPROVER_nondet_size_t();
        __CPROVER_assume(len <= MAX_LEN);

        uint8_t data[MAX_LEN];
        str = aws_string_new_from_array(aws_default_allocator(), data, len);
    }

    aws_string_destroy(str);
}
