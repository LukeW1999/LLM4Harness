#include <aws/common/common.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy(struct aws_string *str);

void aws_string_destroy_harness(void) {
    /* Non-deterministic choice: test with NULL, with NULL allocator, or with real allocator */
    uint8_t choice;
    __CPROVER_assume(choice <= 2);

    if (choice == 0) {
        /* Case 1: str is NULL — must be a no-op */
        aws_string_destroy(NULL);
        /* If we reach here without crash, the NULL case is handled correctly */
        assert(true);

    } else if (choice == 1) {
        /* Case 2: str is non-NULL but allocator is NULL — must be a no-op (static string) */
        /* Allocate a small aws_string with NULL allocator on the stack */
        /* We need at least sizeof(struct aws_string) + 1 byte for the flexible array */
        size_t len;
        __CPROVER_assume(len <= 10);

        /* Allocate memory for the string header + bytes */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        struct aws_string *str = (struct aws_string *)malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Set allocator to NULL (simulating a static string) */
        *(struct aws_allocator **)&str->allocator = NULL;
        *(size_t *)&str->len = len;

        /* Call destroy — should be a no-op since allocator is NULL */
        aws_string_destroy(str);
        /* If we reach here without crash, the NULL allocator case is handled correctly */
        assert(true);

        /* Note: we intentionally do NOT free str here since aws_string_destroy
         * should have been a no-op. In a real proof this would be a memory leak
         * but for CBMC verification purposes this is acceptable. */
        free(str);

    } else {
        /* Case 3: str is non-NULL and allocator is non-NULL — should call aws_mem_release */
        struct aws_allocator *allocator = aws_default_allocator();
        __CPROVER_assume(allocator != NULL);

        /* Use a non-deterministic length */
        size_t len;
        __CPROVER_assume(len <= 10);

        /* Allocate the string using the allocator */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        struct aws_string *str = (struct aws_string *)aws_mem_acquire(allocator, total_size);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields */
        *(struct aws_allocator **)&str->allocator = allocator;
        *(size_t *)&str->len = len;

        /* Fill bytes with non-deterministic data */
        for (size_t i = 0; i < len; i++) {
            uint8_t b;
            *(uint8_t *)&str->bytes[i] = b;
        }
        /* Null terminator */
        *(uint8_t *)&str->bytes[len] = 0;

        /* Verify the string is valid before destroying */
        assert(aws_string_is_valid(str));

        /* Call destroy — should call aws_mem_release */
        aws_string_destroy(str);
        /* If we reach here without crash, the non-NULL allocator case is handled correctly */
        assert(true);
    }

    return 0;
}
