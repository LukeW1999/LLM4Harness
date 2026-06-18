#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness(void) {
    bool is_null = nondet_bool();

    if (is_null) {
        /* Case 1: str is NULL — should be a no-op */
        aws_string_destroy(NULL);
    } else {
        /* Use the default allocator to ensure valid allocator with proper function pointers */
        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);
        __CPROVER_assume(allocator->mem_release != NULL);

        /* Create a valid aws_string using aws_string_new_from_c_str to get a proper dynamic object */
        const char *c_str = "test";
        struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(str->allocator == allocator);

        aws_string_destroy(str);
    }
}
