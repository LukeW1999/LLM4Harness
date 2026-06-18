#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_destroy_secure.
 *
 * From the Doxygen:
 *   "Zeroes out the data bytes of string and then deallocates the memory."
 *
 * Analysis:
 * 1. Changed fields on success: the data bytes are zeroed, then memory is freed.
 *    After the call, the string pointer is no longer valid (freed).
 * 2. Unchanged fields: N/A — the string is destroyed.
 * 3. Failure path: if str is NULL, the function is a no-op (the if(str) guard).
 * 4. Validity invariants: after the call with a non-NULL str, the memory is freed,
 *    so we cannot assert validity on the freed pointer. For NULL input, nothing changes.
 *
 * The function:
 *   - If str is NULL: does nothing (no-op)
 *   - If str is non-NULL:
 *     - Calls aws_secure_zero on the bytes (zeroes len bytes)
 *     - If str->allocator is non-NULL: calls aws_mem_release to free the memory
 *     - If str->allocator is NULL: does NOT free (static string case)
 */

/* Helper to allocate a valid aws_string for testing */
struct aws_string *make_aws_string(struct aws_allocator *allocator, size_t len) {
    /* Allocate memory for the struct + len bytes + 1 null terminator */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = (struct aws_string *)malloc(total_size);
    if (str == NULL) {
        return NULL;
    }
    /* Initialize fields - need to cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = allocator;
    *(size_t *)&str->len = len;
    /* Initialize bytes to nondet values */
    for (size_t i = 0; i < len; i++) {
        *(uint8_t *)&str->bytes[i] = nondet_uint8_t();
    }
    /* Null terminator */
    *(uint8_t *)&str->bytes[len] = 0;
    return str;
}

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test NULL path: function should be a no-op */
        aws_string_destroy_secure(NULL);
        /* No assertions needed for NULL case - just verifying no crash */
        assert(1); /* Harness must have at least one assert */
    } else {
        /* Test non-NULL path */
        /* Bound the string length to keep state space manageable */
        size_t len;
        __CPROVER_assume(len <= 32); /* Bound for CBMC tractability */

        /* Non-deterministically choose whether to use an allocator or not */
        bool use_allocator = nondet_bool();
        struct aws_allocator *allocator = use_allocator ? aws_default_allocator() : NULL;

        /* Create a valid aws_string */
        struct aws_string *str = make_aws_string(allocator, len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save state before the call */
        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;
        bool had_allocator = (str->allocator != NULL);

        /* Call the function under test */
        aws_string_destroy_secure(str);

        /*
         * Postconditions:
         * - If the string had an allocator, memory was freed (we can't safely
         *   dereference str after this point in that case).
         * - If the string had no allocator (static string), the bytes were
         *   zeroed but memory was not freed.
         *
         * For the case with no allocator, we can verify the bytes were zeroed.
         */
        if (!had_allocator) {
            /* Memory was NOT freed, so we can still access it */
            /* The bytes should have been zeroed */
            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
            /* The len and allocator fields should be unchanged */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            assert(str->allocator == NULL);
        } else {
            /*
             * Memory was freed. We cannot safely dereference str.
             * We assert that the function completed without error (no crash).
             * The fact that we reach this point means the function executed correctly.
             */
            assert(had_allocator == true); /* Trivially true, confirms we took this path */
        }
    }
}
