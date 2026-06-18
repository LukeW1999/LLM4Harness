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
 * Behavior:
 * - If str is NULL, nothing happens (no-op).
 * - If str->allocator is NULL (static string), nothing happens (no-op).
 * - If str is non-NULL and str->allocator is non-NULL, the memory is released.
 *
 * The function returns void, so there is no return value to check.
 * The key postcondition is that the function does not crash/undefined-behavior
 * in any of the three cases above.
 *
 * For CBMC purposes, we verify:
 * 1. The function can be called with NULL without crashing.
 * 2. The function can be called with a valid string with NULL allocator without crashing.
 * 3. The function can be called with a valid string with non-NULL allocator without crashing.
 */

/* Helper to create a valid aws_string non-deterministically */
struct aws_string *make_nondet_aws_string(void) {
    /* We need to allocate an aws_string with some bytes following it.
     * Use a bounded length for the proof. */
    size_t len;
    __CPROVER_assume(len <= 10); /* bound the string length */

    /* Allocate memory for the struct + len + 1 bytes (null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }

    /* Initialize the const fields via pointer casting */
    struct aws_string *mutable_str = (struct aws_string *)str;
    /* Set len */
    *((size_t *)&mutable_str->len) = len;
    /* bytes are already allocated after the struct, leave them nondet */

    return mutable_str;
}

void aws_string_destroy_harness(void) {
    /* Non-deterministically choose which case to test */
    bool is_null = nondet_bool();
    bool has_allocator = nondet_bool();

    if (is_null) {
        /* Case 1: str is NULL — should be a no-op */
        aws_string_destroy(NULL);
        /* If we reach here without crashing, the test passes */
        assert(true);
    } else {
        /* Case 2 & 3: str is non-NULL */
        /* Allocate a string with some data */
        size_t len;
        __CPROVER_assume(len <= 10);

        /* Allocate memory for struct + bytes + null terminator */
        void *mem = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(mem != NULL);

        struct aws_string *str = (struct aws_string *)mem;

        /* Set the len field */
        *((size_t *)&str->len) = len;

        if (has_allocator) {
            /* Case 3: str->allocator is non-NULL — memory will be released */
            struct aws_allocator *allocator = aws_default_allocator();
            *((struct aws_allocator **)&str->allocator) = allocator;

            /* Snapshot allocator pointer before call */
            struct aws_allocator *allocator_before = str->allocator;
            size_t len_before = str->len;

            /* The function should release the memory without crashing */
            aws_string_destroy(str);

            /* After destroy with allocator, we cannot safely dereference str.
             * The key assertion is that we reached here without undefined behavior. */
            assert(true);
        } else {
            /* Case 2: str->allocator is NULL — should be a no-op */
            *((struct aws_allocator **)&str->allocator) = NULL;

            /* Snapshot fields before call */
            size_t len_before = str->len;
            const uint8_t *bytes_before = str->bytes;

            /* Call destroy — should be a no-op since allocator is NULL */
            aws_string_destroy(str);

            /* Since it was a no-op, str should still be accessible and unchanged */
            assert(str->allocator == NULL);
            assert(str->len == len_before);
            assert(str->bytes == bytes_before);

            /* Clean up manually since destroy was a no-op */
            free(mem);
        }
    }
}
