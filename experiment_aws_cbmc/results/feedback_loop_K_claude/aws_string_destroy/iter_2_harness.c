#include <aws/common/common.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_destroy(struct aws_string *str);

void aws_string_destroy_harness(void) {
    /* Non-deterministic choice: test with NULL or with a valid string */
    bool is_null;

    if (is_null) {
        /* Case 1: str is NULL — must be a no-op */
        aws_string_destroy(NULL);

    } else {
        /* Case 2: str is non-NULL and valid */
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
        __CPROVER_assume(aws_string_is_valid(str));

        /* Call destroy — should call aws_mem_release */
        aws_string_destroy(str);
    }
}
