#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_string_destroy.
 *
 * From the implementation:
 *   void aws_string_destroy(struct aws_string *str) {
 *       if (str && str->allocator) {
 *           aws_mem_release(str->allocator, str);
 *       }
 *   }
 *
 * The function:
 * 1. If str is NULL, does nothing (no-op).
 * 2. If str->allocator is NULL, does nothing (static string, no-op).
 * 3. If str is non-NULL and str->allocator is non-NULL, releases the memory.
 *
 * Since the function frees memory, we cannot assert on the string's fields
 * after the call when it was freed. We verify:
 * - The function does not crash on NULL input.
 * - The function does not crash on a valid string with NULL allocator.
 * - The function does not crash on a valid string with a real allocator.
 */

void aws_string_destroy_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test NULL path: should be a no-op */
        aws_string_destroy(NULL);
        /* If we get here without crashing, the NULL case is handled correctly */
        assert(true);
    } else {
        /* Create a valid aws_string */
        /* We need to allocate memory for the struct + bytes */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the string header + len bytes + 1 null terminator */
        /* sizeof(struct aws_string) already includes 1 byte for bytes[1] */
        size_t total_size = sizeof(struct aws_string) + len;
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size <= sizeof(struct aws_string) + MAX_BUFFER_SIZE);

        struct aws_string *str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Non-deterministically choose allocator: NULL (static) or real allocator */
        bool has_allocator = nondet_bool();

        /* We need to write to the const fields - use a cast */
        struct aws_string *mutable_str = (struct aws_string *)str;

        if (has_allocator) {
            /* Use a real allocator so the string will be freed */
            struct aws_allocator *allocator = aws_default_allocator();
            /* Write the allocator field */
            *(struct aws_allocator **)&mutable_str->allocator = allocator;
            *(size_t *)&mutable_str->len = len;

            /* 
             * When has_allocator is true, aws_string_destroy will call
             * aws_mem_release which frees the memory. We cannot access
             * str after this point.
             * 
             * We verify the function completes without error.
             * Since we allocated with malloc (not the aws allocator),
             * we need to use the aws allocator to allocate the string
             * so that aws_mem_release works correctly.
             *
             * Actually, let's allocate with the aws allocator instead.
             */
            /* Re-allocate using aws allocator */
            free(str);
            str = aws_mem_acquire(allocator, total_size);
            __CPROVER_assume(str != NULL);
            *(struct aws_allocator **)&str->allocator = allocator;
            *(size_t *)&str->len = len;

            /* Call the function - it will free str */
            aws_string_destroy(str);
            /* str is now freed; we cannot dereference it */
            /* If we reach here without undefined behavior, the test passes */
            assert(true);
        } else {
            /* NULL allocator: static string, destroy is a no-op */
            *(struct aws_allocator **)&mutable_str->allocator = NULL;
            *(size_t *)&mutable_str->len = len;

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
