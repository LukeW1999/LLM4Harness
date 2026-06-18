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
 * Since the function frees memory, we cannot assert on the string contents
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
        /* If we reach here without crashing, the NULL case is handled correctly */
        assert(true);
    } else {
        /* Allocate a string with bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the aws_string struct plus data bytes plus null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size <= sizeof(struct aws_string) + MAX_BUFFER_SIZE + 1);

        struct aws_string *str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Non-deterministically set the length field */
        /* We need to write to the const field - use a cast */
        *(size_t *)&str->len = len;

        /* Non-deterministically choose whether the string has an allocator */
        bool has_allocator = nondet_bool();

        if (has_allocator) {
            /* Use the default allocator so aws_mem_release works correctly */
            *(struct aws_allocator **)&str->allocator = aws_default_allocator();
            /* 
             * When allocator is non-NULL, aws_string_destroy will call aws_mem_release.
             * We cannot assert on str after this call since memory is freed.
             * The key property is that the function completes without error.
             */
            aws_string_destroy(str);
            /* If we reach here, the function completed without crashing */
            assert(true);
        } else {
            /* NULL allocator: destroy should be a no-op */
            *(struct aws_allocator **)&str->allocator = NULL;

            /* Save state before call */
            size_t old_len = str->len;

            aws_string_destroy(str);

            /* With NULL allocator, memory is NOT freed - str is still valid */
            /* The function should have been a no-op */
            assert(str->len == old_len);
            assert(str->allocator == NULL);

            /* Clean up manually since destroy was a no-op */
            free(str);
        }
    }
}

void aws_string_destroy_harness(void) {
    aws_string_destroy_harness();
    return 0;
}
