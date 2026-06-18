#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

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
 * - If str is NULL: no-op (safe to call with NULL)
 * - If str->allocator is NULL: no-op (static string, not freed)
 * - If str->allocator is non-NULL: releases the memory
 *
 * Postconditions:
 * - The function does not crash (no undefined behavior)
 * - No return value to check
 * - If str is NULL or str->allocator is NULL, nothing changes
 */

void aws_string_destroy_harness() {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test with NULL pointer — should be a no-op */
        aws_string_destroy(NULL);
        /* If we get here without crashing, the NULL case is handled correctly */
        assert(true);
    } else {
        /* Create a non-deterministic aws_string */
        /* We need to allocate memory for the struct + bytes */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the string header + data bytes + null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size <= MAX_BUFFER_SIZE + sizeof(struct aws_string) + 1);

        struct aws_string *str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Set up the string fields — they are const so we use a cast trick */
        /* We write through a non-const pointer to initialize */
        struct aws_string *mutable_str = str;

        /* Non-deterministically choose allocator: NULL (static) or real allocator */
        bool has_allocator = nondet_bool();

        /* Initialize the length field */
        *(size_t *)(&mutable_str->len) = len;

        if (has_allocator) {
            /* Use the default allocator so aws_mem_release works correctly */
            struct aws_allocator *allocator = aws_default_allocator();
            *(struct aws_allocator **)(&mutable_str->allocator) = allocator;

            /* For the case with a real allocator, we need to use aws_mem_alloc
             * so that aws_mem_release can properly free it.
             * Re-allocate using the aws allocator. */
            struct aws_string *aws_str = aws_mem_acquire(allocator, total_size);
            __CPROVER_assume(aws_str != NULL);

            *(struct aws_allocator **)(&aws_str->allocator) = allocator;
            *(size_t *)(&aws_str->len) = len;

            /* Initialize bytes to avoid uninitialized memory issues */
            for (size_t i = 0; i < len; i++) {
                *(uint8_t *)(&aws_str->bytes[i]) = nondet_uint8_t();
            }
            *(uint8_t *)(&aws_str->bytes[len]) = 0; /* null terminator */

            /* Verify the string is valid before destroying */
            assert(aws_string_is_valid(aws_str));

            /* Call destroy — should free the memory */
            aws_string_destroy(aws_str);

            /* After destroy with allocator, memory is freed — no further assertions
             * on the pointer as it's now invalid. The key assertion is that
             * we reached here without crashing. */
            assert(true);

            /* Free the malloc'd str since we didn't use it */
            free(str);
        } else {
            /* NULL allocator — static string, destroy should be a no-op */
            *(struct aws_allocator **)(&mutable_str->allocator) = NULL;

            /* Initialize bytes */
            for (size_t i = 0; i < len; i++) {
                *(uint8_t *)(&mutable_str->bytes[i]) = nondet_uint8_t();
            }
            *(uint8_t *)(&mutable_str->bytes[len]) = 0; /* null terminator */

            /* Verify the string is valid before destroying */
            assert(aws_string_is_valid(str));

            /* Save state before call */
            size_t old_len = str->len;

            /* Call destroy — should be a no-op since allocator is NULL */
            aws_string_destroy(str);

            /* After no-op destroy, the string memory is still valid */
            /* The len field should be unchanged */
            assert(str->len == old_len);
            assert(str->allocator == NULL);

            /* String should still be valid */
            assert(aws_string_is_valid(str));

            free(str);
        }
    }
}
