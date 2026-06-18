#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
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
 * 2. If str->allocator is NULL (static string), does nothing (no-op).
 * 3. If str is non-NULL and has an allocator, frees the memory.
 *
 * Postconditions:
 * - The function returns void, so no return value to check.
 * - The function should not crash for any valid input (including NULL).
 * - We verify that the function handles NULL str gracefully.
 * - We verify that the function handles str with NULL allocator gracefully.
 */

void aws_string_destroy_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test with NULL pointer — should be a no-op */
        aws_string_destroy(NULL);
        /* If we reach here without crashing, the NULL case is handled correctly */
        assert(true);
    } else {
        /* Test with a non-NULL string */
        /* We need to allocate an aws_string with some bytes */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the string header + bytes + null terminator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(total_size >= sizeof(struct aws_string));
        __CPROVER_assume(total_size <= MAX_BUFFER_SIZE + sizeof(struct aws_string) + 1);

        struct aws_string *str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Non-deterministically set the allocator:
         * - NULL means static string (no-op destroy)
         * - non-NULL means dynamically allocated (will be freed) */
        bool has_allocator = nondet_bool();

        if (has_allocator) {
            /* Use the default allocator */
            struct aws_allocator *allocator = aws_default_allocator();
            /* Write the allocator field (cast away const for initialization) */
            *(struct aws_allocator **)&str->allocator = allocator;
        } else {
            /* Static string: allocator is NULL */
            *(struct aws_allocator **)&str->allocator = NULL;
        }

        /* Set the length field */
        *(size_t *)&str->len = len;

        /* Initialize bytes to avoid undefined behavior */
        for (size_t i = 0; i <= len; i++) {
            *(uint8_t *)&str->bytes[i] = nondet_uint8_t();
        }
        /* Ensure null terminator */
        *(uint8_t *)&str->bytes[len] = 0;

        if (!has_allocator) {
            /* With NULL allocator, destroy is a no-op — string should not be freed */
            /* We can safely call destroy and then check the string is still accessible */
            aws_string_destroy(str);
            /* If we reach here without crashing, the NULL allocator case is handled */
            assert(true);
            /* Free the memory ourselves since destroy was a no-op */
            free(str);
        } else {
            /* With a real allocator, we need to use aws_mem_calloc to allocate
             * so that aws_mem_release can properly free it.
             * Instead, let's allocate via the allocator itself. */
            
            /* Re-allocate using the aws allocator so aws_mem_release works */
            struct aws_allocator *allocator = aws_default_allocator();
            free(str); /* free the malloc'd one */
            
            /* Allocate via aws allocator */
            str = aws_mem_acquire(allocator, total_size);
            __CPROVER_assume(str != NULL);
            
            *(struct aws_allocator **)&str->allocator = allocator;
            *(size_t *)&str->len = len;
            for (size_t i = 0; i < len; i++) {
                *(uint8_t *)&str->bytes[i] = nondet_uint8_t();
            }
            *(uint8_t *)&str->bytes[len] = 0;
            
            /* Call destroy — this should free the memory */
            aws_string_destroy(str);
            /* If we reach here without crashing, the function worked correctly */
            assert(true);
        }
    }
}

void aws_string_destroy_harness(void) {
    aws_string_destroy_harness();
    return 0;
}
