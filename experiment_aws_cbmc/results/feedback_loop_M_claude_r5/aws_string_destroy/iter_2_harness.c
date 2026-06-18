#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);

    if (is_null) {
        /* Test NULL path: should be a no-op */
        aws_string_destroy(NULL);
        assert(true);
    } else {
        bool has_allocator;
        __CPROVER_assume(has_allocator == 0 || has_allocator == 1);

        if (has_allocator) {
            /* Use a real allocator so the string will be freed */
            struct aws_allocator *allocator = aws_default_allocator();

            size_t len;
            __CPROVER_assume(len <= 16);

            size_t total_size = sizeof(struct aws_string) + len;

            struct aws_string *str = aws_mem_acquire(allocator, total_size);
            __CPROVER_assume(str != NULL);

            *(struct aws_allocator **)&str->allocator = allocator;
            *(size_t *)&str->len = len;

            /* Call the function - it will free str */
            aws_string_destroy(str);
            /* str is now freed; we cannot dereference it */
            assert(true);
        } else {
            /* NULL allocator: static string, destroy is a no-op */
            size_t len;
            __CPROVER_assume(len <= 16);

            size_t total_size = sizeof(struct aws_string) + len;

            struct aws_string *str = malloc(total_size);
            __CPROVER_assume(str != NULL);

            *(struct aws_allocator **)&str->allocator = NULL;
            *(size_t *)&str->len = len;

            /* Save state before call */
            size_t old_len = str->len;

            /* Call the function - should be a no-op */
            aws_string_destroy(str);

            /* str should be unchanged since allocator was NULL */
            assert(str->allocator == NULL);
            assert(str->len == old_len);

            free(str);
        }
    }
}

void aws_string_destroy_harness(void) {
    aws_string_destroy_harness();
    return 0;
}
