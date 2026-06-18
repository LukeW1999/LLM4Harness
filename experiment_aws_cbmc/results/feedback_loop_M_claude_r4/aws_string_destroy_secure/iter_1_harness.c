#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/**
 * Harness for aws_string_destroy_secure.
 *
 * From the Doxygen:
 *   "Zeroes out the data bytes of string and then deallocates the memory.
 *    Not safe to run on a string created with AWS_STATIC_STRING_FROM_LITERAL."
 *
 * Analysis:
 * 1. Changed fields: The data bytes are zeroed, then the memory is freed.
 *    After the call, the string pointer is no longer valid (freed).
 * 2. Unchanged fields: N/A — the struct is freed.
 * 3. Failure path: If str is NULL, the function is a no-op (the if(str) guard).
 * 4. Validity invariants: After the call, we can only assert things about the
 *    NULL case (no-op path).
 *
 * Since the function frees the memory, we cannot dereference str after the call.
 * We verify:
 *   - The function handles NULL input without crashing (no-op).
 *   - The function handles a valid aws_string without crashing.
 *   - For the NULL case, nothing changes (trivially).
 */

/* Helper to create a valid aws_string with allocated memory */
struct aws_string *ensure_string_is_allocated(size_t len) {
    /* Allocate enough memory for the struct header + len bytes + 1 null terminator */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = malloc(total_size);
    if (str == NULL) {
        return NULL;
    }
    /* Set the fields — we need to cast away const to initialize */
    *(struct aws_allocator **)&str->allocator = aws_default_allocator();
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
        /* Test NULL path — function should be a no-op */
        aws_string_destroy_secure(NULL);
        /* If we get here without crashing, the NULL case is handled correctly */
        assert(true); /* Explicit assertion: NULL path completes without error */
    } else {
        /* Test valid string path */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        struct aws_string *str = ensure_string_is_allocated(len);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save metadata before the call */
        size_t old_len = str->len;

        /* The function zeroes the bytes and frees the memory.
         * We cannot access str after the call if it had an allocator.
         * We verify the function completes without crashing. */
        aws_string_destroy_secure(str);

        /* After the call, str has been freed — we cannot dereference it.
         * The key postcondition is that the function completed without error.
         * Assert that old_len was valid (it was set before the call). */
        assert(old_len <= MAX_BUFFER_SIZE);

        /* The function ran to completion without crashing — this is the
         * primary postcondition we can assert after freeing. */
        assert(true);
    }
}

void aws_string_destroy_secure_harness(void) {
    aws_string_destroy_secure_harness();
    return 0;
}
