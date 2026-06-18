#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_destroy_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);

    if (is_null) {
        /* Test with NULL pointer — should be a no-op */
        aws_string_destroy(NULL);
        assert(true);
    } else {
        size_t len;
        __CPROVER_assume(len <= 16);

        /* Allocate memory for the string header + bytes + null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;

        bool has_allocator;
        __CPROVER_assume(has_allocator == 0 || has_allocator == 1);

        if (!has_allocator) {
            /* Static string: allocator is NULL, destroy is a no-op */
            struct aws_string *str = malloc(total_size);
            __CPROVER_assume(str != NULL);

            *(struct aws_allocator **)&str->allocator = NULL;
            *(size_t *)&str->len = len;
            *(uint8_t *)&str->bytes[len] = 0;

            aws_string_destroy(str);
            assert(true);
            free(str);
        } else {
            /* Allocate via aws allocator so aws_mem_release works */
            struct aws_allocator *allocator = aws_default_allocator();

            struct aws_string *str = aws_mem_acquire(allocator, total_size);
            __CPROVER_assume(str != NULL);

            *(struct aws_allocator **)&str->allocator = allocator;
            *(size_t *)&str->len = len;
            *(uint8_t *)&str->bytes[len] = 0;

            aws_string_destroy(str);
            assert(true);
        }
    }
}

void aws_string_destroy_harness(void) {
    aws_string_destroy_harness();
    return 0;
}
