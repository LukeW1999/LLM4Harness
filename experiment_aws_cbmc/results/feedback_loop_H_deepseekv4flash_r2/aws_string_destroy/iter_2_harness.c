#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness() {
    struct aws_string *str = nondet_allocate_string();
    if (str) {
        __CPROVER_assume(aws_string_is_valid(str));
        /* Optionally set allocator to NULL to test that branch */
        if (nondet_bool()) {
            struct aws_allocator *null_alloc = NULL;
            memcpy((void *)&str->allocator, &null_alloc, sizeof(null_alloc));
        }
    }
    aws_string_destroy(str);
}
