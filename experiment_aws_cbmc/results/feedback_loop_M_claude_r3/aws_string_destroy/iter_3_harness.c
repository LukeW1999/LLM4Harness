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
        /* Allocate a string with bounded length */
        size_t len;
        __CPROVER_assume(len <= 16);

        /* Allocate memory for the aws_string struct plus data bytes plus null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;

        struct aws_string *str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Write to the const fields using casts */
        *(size_t *)&str->len = len;

        /* Non-deterministically choose whether the string has an allocator */
        bool has_allocator;
        __CPROVER_assume(has_allocator == 0 || has_allocator == 1);

        if (has_allocator) {
            /* Use the default allocator so aws_mem_release works correctly */
            struct aws_allocator *allocator = aws_default_allocator();
            *(struct aws_allocator **)&str->allocator = allocator;
            /*
             * When allocator is non-NULL, aws_string_destroy will call aws_mem_release.
             * We cannot assert on str after this call since memory is freed.
             */
            aws_string_destroy(str);
            assert(true);
        } else {
            /* NULL allocator: destroy should be a no-op */
            *(struct aws_allocator **)&str->allocator = NULL;

            /* Save state before call */
            size_t old_len = str->len;

            aws_string_destroy(str);

            /* With NULL allocator, memory is NOT freed - str is still valid */
            assert(str->len == old_len);
            assert(str->allocator == NULL);

            /* Clean up manually since destroy was a no-op */
            free(str);
        }
    }
}
