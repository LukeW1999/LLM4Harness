#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
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
 * 1. If str is NULL, does nothing (no-op).
 * 2. If str->allocator is NULL, does nothing (static string, no-op).
 * 3. If str is non-NULL and str->allocator is non-NULL, releases the memory.
 *
 * There is no return value to check.
 * The function is a destructor — after calling it, we cannot dereference str
 * if memory was freed. We can only assert that the call did not crash.
 */
void aws_string_destroy_harness(void) {
    /* Non-deterministically choose whether str is NULL */
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        /* Create a valid aws_string with non-deterministic content */
        /* We need to allocate memory for the string header + bytes */
        /* Use a bounded length */
        size_t len;
        __CPROVER_assume(len <= 16); /* bound the string length */

        /* Allocate memory for the string: header + len + 1 (null terminator) */
        /* The struct has a flexible-array-like member bytes[1], so we need
         * sizeof(struct aws_string) + len bytes total (bytes[0] is already in struct) */
        size_t alloc_size = sizeof(struct aws_string) + len;
        __CPROVER_assume(alloc_size >= sizeof(struct aws_string));
        __CPROVER_assume(alloc_size <= sizeof(struct aws_string) + 16);

        str = malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* Non-deterministically set the allocator:
         * - NULL means static string (no-op destroy)
         * - non-NULL means dynamic string (will be freed) */
        struct aws_allocator *alloc = nondet_bool() ? NULL : aws_default_allocator();

        /* We need to write to the const fields — use a cast */
        struct aws_string *mutable_str = (struct aws_string *)str;
        /* Write allocator field */
        *(struct aws_allocator **)&mutable_str->allocator = alloc;
        /* Write len field */
        *(size_t *)&mutable_str->len = len;
        /* Write some bytes (non-deterministic) */
        /* bytes are already non-deterministic from malloc */

        /* Assume the string is valid before calling destroy */
        __CPROVER_assume(aws_string_is_valid(str));
    }
    /* else str remains NULL */

    /* Record whether str was NULL or had a NULL allocator before the call,
     * to determine if memory will be freed */
    bool str_is_null = (str == NULL);
    bool allocator_is_null = (!str_is_null && str->allocator == NULL);
    bool will_free = (!str_is_null && !allocator_is_null);

    /* Call the function under test */
    aws_string_destroy(str);

    /* Postconditions:
     * 1. If str was NULL, nothing happened — no crash.
     * 2. If str->allocator was NULL, nothing happened — no crash.
     * 3. If str was freed, we cannot dereference it anymore.
     *
     * The key assertion is that the function completed without error.
     * We assert the conditions that led to the no-op cases.
     */

    if (str_is_null) {
        /* str was NULL, function was a no-op */
        assert(str == NULL);
    } else if (allocator_is_null) {
        /* str->allocator was NULL, function was a no-op */
        /* str pointer itself is unchanged (we didn't free it) */
        assert(str != NULL);
        /* The string's allocator is still NULL */
        assert(str->allocator == NULL);
        /* The string's len is unchanged */
        /* (we can still read str since it wasn't freed) */
        assert(aws_string_is_valid(str));
        /* Clean up manually since destroy was a no-op */
        free(str);
    } else {
        /* Memory was freed — we cannot dereference str.
         * The only assertion we can make is that we reached this point
         * without crashing (implicit in CBMC). */
        assert(will_free);
    }
}
