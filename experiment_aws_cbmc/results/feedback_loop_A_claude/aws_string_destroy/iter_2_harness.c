#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        size_t alloc_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(alloc_size >= sizeof(struct aws_string) + 1);

        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        struct aws_allocator *allocator;
        if (nondet_bool()) {
            allocator = NULL;
        } else {
            allocator = aws_default_allocator();
        }

        *(struct aws_allocator **)&str->allocator = allocator;
        *(size_t *)&str->len = len;

        __CPROVER_assume(aws_string_is_valid(str));

        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;

        if (str->allocator == NULL) {
            aws_string_destroy(str);
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            assert(aws_string_is_valid(str));
        } else {
            aws_string_destroy(str);
            assert(1);
        }
    } else {
        aws_string_destroy(NULL);
        assert(str == NULL);
    }
}
