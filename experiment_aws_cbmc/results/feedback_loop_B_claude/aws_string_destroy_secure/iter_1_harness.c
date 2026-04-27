#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>

/**
 * aws_string_destroy_secure:
 *   - If str is non-NULL:
 *       1. Zeroes out the bytes of the string (aws_secure_zero on str->len bytes)
 *       2. If str->allocator is non-NULL, releases the memory via aws_mem_release
 *   - If str is NULL: does nothing (no-op)
 *
 * This function has no return value (void).
 * The key observable postcondition is that if str != NULL, the bytes are zeroed.
 * After the call, if str->allocator != NULL, the memory is freed (we can't
 * dereference it safely). If str->allocator == NULL, the struct still exists
 * and its bytes should be zeroed.
 */

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose whether to pass NULL or a valid string */
    struct aws_string *str = NULL;

    if (nondet_bool()) {
        /* Allocate a bounded aws_string for testing */
        /* We need to create an aws_string with a bounded length */
        size_t len;
        __CPROVER_assume(len <= 8); /* bound the string length for tractability */

        /* Allocate memory for the string: sizeof(aws_string) - 1 + len + 1 (null terminator) */
        size_t alloc_size = sizeof(struct aws_string) + len;
        __CPROVER_assume(alloc_size >= sizeof(struct aws_string));
        __CPROVER_assume(alloc_size <= sizeof(struct aws_string) + 8);

        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields - need to cast away const for initialization */
        *(struct aws_allocator **)&str->allocator = nondet_bool() ? can_fail_allocator() : NULL;
        *(size_t *)&str->len = len;

        /* The bytes are already allocated as part of the struct */
        /* aws_string_is_valid requires bytes[len] == '\0' */
        /* Initialize bytes to non-deterministic values */
        for (size_t i = 0; i < len; i++) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)str->bytes)[len] = '\0';

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Save state before the call */
    bool str_is_null = (str == NULL);
    struct aws_allocator *old_allocator = str_is_null ? NULL : str->allocator;
    size_t old_len = str_is_null ? 0 : str->len;

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Postconditions */
    if (str_is_null) {
        /* NULL input: function is a no-op, nothing to assert about str */
        /* Just verify we didn't crash */
        assert(true);
    } else {
        /* Non-NULL input: bytes should have been zeroed */
        if (old_allocator == NULL) {
            /* Memory was NOT freed (no allocator), so we can still read the struct */
            /* The bytes should be zeroed out */
            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
            /* The len and allocator fields are not modified by this function */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
        } else {
            /* Memory was freed via aws_mem_release - we cannot safely dereference str */
            /* Just assert that we reached this point without crashing */
            assert(true);
        }
    }
}
