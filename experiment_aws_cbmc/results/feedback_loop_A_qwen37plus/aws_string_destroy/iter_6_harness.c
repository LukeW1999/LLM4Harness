#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    struct aws_string s;
    if (nondet_bool()) {
        str = &s;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= 1024);
    }
    aws_string_destroy(str);
}
