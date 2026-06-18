#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    bool is_null;
    __CPROVER_assume(is_null == is_null); /* suppress uninitialized warning */

    if (!is_null) {
        size_t len;
        __CPROVER_assume(len <= 10);

        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        bool use_allocator;
        struct aws_allocator *allocator = aws_default_allocator();
        *(struct aws_allocator **)&str->allocator = use_allocator ? allocator : NULL;
        *(size_t *)&str->len = len;
    }

    bool str_is_null = (str == NULL);
    bool allocator_is_null = (!str_is_null && str->allocator == NULL);

    aws_string_destroy(str);

    if (str_is_null) {
        assert(str == NULL);
    }

    if (!str_is_null && allocator_is_null) {
        assert(str != NULL);
        assert(str->allocator == NULL);
    }
}
