#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        struct aws_string s;
        str = &s;
        str->allocator = aws_default_allocator();
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= 1024);
    }
    aws_string_destroy(str);
}
