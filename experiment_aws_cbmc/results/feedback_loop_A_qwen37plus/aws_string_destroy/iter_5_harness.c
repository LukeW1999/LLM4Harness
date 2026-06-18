#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 1024);
        str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        str->allocator = aws_default_allocator();
        str->len = len;
        str->bytes[len] = 0;
    }
    aws_string_destroy(str);
}
