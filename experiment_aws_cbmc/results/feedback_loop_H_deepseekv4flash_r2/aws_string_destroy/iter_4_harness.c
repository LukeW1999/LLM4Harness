#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_destroy_harness() {
    struct aws_string *str = nondet_allocate_string();
    if (str) {
        __CPROVER_assume(aws_string_is_valid(str));
        str->allocator = aws_default_allocator();
    }
    aws_string_destroy(str);
}
