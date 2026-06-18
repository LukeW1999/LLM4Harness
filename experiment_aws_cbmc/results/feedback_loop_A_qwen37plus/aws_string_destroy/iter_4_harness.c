#include <aws/common/allocator.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 1024);
        str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;
        str->bytes[len] = 0;
    }
    aws_string_destroy(str);
}
