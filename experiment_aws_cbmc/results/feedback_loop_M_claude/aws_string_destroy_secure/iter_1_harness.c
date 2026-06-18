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
 * Harness for aws_string_destroy_secure.
 *
 * From the Doxygen:
 * "Zeroes out the data bytes of string and then deallocates the memory.
 *  Not safe to run on a string created with AWS_STATIC_STRING_FROM_LITERAL."
 *
 * Analysis:
 * 1. Changed fields: The data bytes are zeroed, then memory is freed (if allocator != NULL).
 * 2. Unchanged fields: N/A — the string is destroyed.
 * 3. Failure path: If str is NULL, function is a no-op (no crash).
 * 4. Validity invariants: After the call, if str was NULL, nothing changes.
 *    If str was non-NULL with an allocator, memory is freed.
 *    If str was non-NULL without an allocator, bytes are zeroed but not freed.
 */

/* Helper to create a valid aws_string with allocated memory */
struct aws_string *ensure_string_is_allocated(size_t len) {
    /* Allocate enough memory for the struct + len bytes + 1 null terminator */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = malloc(total_size);
    if (str == NULL) {
        return NULL;
    }
    /* Set the fields — need to cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = nondet_bool() ? aws_default_allocator() : NULL;
    *(size_t *)&str->len = len;
    /* bytes are already allocated as part of the struct */
    return str;
}

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test NULL path — should be a no-op */
        aws_string_destroy_secure(NULL);
        /* No assertions needed for NULL case — just verify no crash */
    } else {
        /* Test non-NULL path */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate the string manually to control memory layout */
        size_t total_size = sizeof(struct aws_string) + len + 1;
        __CPROVER_assume(total_size >= sizeof(struct aws_string) + 1); /* no overflow */
        __CPROVER_assume(total_size <= MAX_BUFFER_SIZE + sizeof(struct aws_string) + 1);

        struct aws_string *str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields */
        bool has_allocator = nondet_bool();
        *(struct aws_allocator **)&str->allocator = has_allocator ? aws_default_allocator() : NULL;
        *(size_t *)&str->len = len;

        /* Fill bytes with nondet data */
        for (size_t i = 0; i < len + 1; i++) {
            *(uint8_t *)&str->bytes[i] = nondet_uint8_t();
        }

        /* Verify the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save relevant state before the call */
        bool had_allocator = (str->allocator != NULL);
        size_t old_len = str->len;

        /* Call the function under test */
        aws_string_destroy_secure(str);

        /*
         * Postconditions:
         * - If str had no allocator, the memory was NOT freed (bytes zeroed but struct still accessible).
         *   We can verify the bytes were zeroed.
         * - If str had an allocator, the memory was freed — we cannot safely access it.
         *
         * Since CBMC tracks memory, we assert based on the allocator presence.
         */
        if (!had_allocator) {
            /* Memory was not freed, bytes should be zeroed */
            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
        }
        /* If had_allocator, memory is freed — no safe assertions on str content */
    }
}
