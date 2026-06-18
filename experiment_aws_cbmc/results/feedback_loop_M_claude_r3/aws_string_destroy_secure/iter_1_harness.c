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
 * 3. Failure path: If str is NULL, function is a no-op.
 * 4. Validity invariants: After call, str is freed (or was NULL).
 *
 * The function:
 *   - If str is NULL: no-op
 *   - If str is non-NULL:
 *     - Zeroes the bytes (aws_secure_zero on str->bytes for str->len bytes)
 *     - If str->allocator != NULL: frees the memory
 */

/* Helper to create a valid aws_string with allocated memory */
struct aws_string *make_aws_string(struct aws_allocator *allocator, size_t len) {
    /* Allocate enough memory for the struct plus len+1 bytes (for null terminator) */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    struct aws_string *str = (struct aws_string *)malloc(total_size);
    if (str == NULL) {
        return NULL;
    }
    /* Initialize the fields — need to cast away const for initialization */
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
        /* No assertions needed for NULL case — just verify no crash */
        assert(true);
    } else {
        /* Test non-NULL path */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Non-deterministically choose whether the string has an allocator */
        bool has_allocator = nondet_bool();
        struct aws_allocator *allocator = has_allocator ? aws_default_allocator() : NULL;

        struct aws_string *str = make_aws_string(allocator, len);
        __CPROVER_assume(str != NULL);

        /* Verify the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save relevant state before the call */
        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;

        /* The function zeroes bytes and frees if allocator is set.
         * We can only check the zeroing if allocator is NULL (memory not freed). */
        bool will_free = (old_allocator != NULL);

        if (!will_free) {
            /* If no allocator, memory won't be freed, so we can check zeroing */
            aws_string_destroy_secure(str);

            /* After zeroing, all bytes should be zero */
            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
            /* len and allocator fields should be unchanged (they're const) */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            assert(str->allocator == NULL);

            /* Free the memory ourselves since the function didn't */
            free(str);
        } else {
            /* If allocator is set, memory will be freed after zeroing.
             * We can't safely access str after the call.
             * Just verify the function doesn't crash. */
            aws_string_destroy_secure(str);
            /* str is now freed — we cannot dereference it */
            /* Just assert that we reached this point without crashing */
            assert(true);
        }
    }
}

void aws_string_destroy_secure_harness(void) {
    aws_string_destroy_secure_harness();
    return 0;
}
