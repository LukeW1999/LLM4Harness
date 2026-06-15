#include <aws/common/string.h>
#include <stdlib.h>

int nondet_bool(void);

void aws_string_destroy_harness() {
    struct aws_string *str = NULL;
    int has_allocator = 0;

    if (nondet_bool()) {
        // str is NULL
        str = NULL;
    } else {
        size_t len;
        __CPROVER_assume(len <= 64);
        size_t total_size = sizeof(struct aws_string) + len + 1;
        str = malloc(total_size);
        __CPROVER_assume(str != NULL);
        str->len = len;
        if (nondet_bool()) {
            str->allocator = malloc(sizeof(struct aws_allocator));
            __CPROVER_assume(str->allocator != NULL);
            has_allocator = 1;
        } else {
            str->allocator = NULL;
        }
    }

    aws_string_destroy(str);

    if (str == NULL) {
        assert(1);
    } else {
        if (has_allocator) {
            assert(!AWS_MEM_IS_READABLE(str, sizeof(struct aws_string)));
        } else {
            assert(AWS_MEM_IS_READABLE(str, sizeof(struct aws_string)));
        }
    }
}
