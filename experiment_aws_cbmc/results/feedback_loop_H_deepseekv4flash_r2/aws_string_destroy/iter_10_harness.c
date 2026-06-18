#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_string_destroy_harness() {
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_string *str = aws_string_new_from_c_str(allocator, "test");
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));
    aws_string_destroy(str);
}
