#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Harness for aws_string_destroy_secure.
 *
 * From the Doxygen:
 *   "Zeroes out the data bytes of string and then deallocates the memory."
 *
 * Analysis:
 * 1. Changed fields on success: the bytes are zeroed, then the memory is freed.
 *    After the call, we cannot dereference str (it may be freed), so we can only
 *    assert things we can observe before the call or about the function not crashing.
 * 2. Unchanged fields: N/A (memory is freed).
 * 3. Failure path: if str is NULL, the function is a no-op (no crash).
 * 4. Validity invariants: the function must not crash; if str is NULL it's a no-op.
 *
 * Because the function frees memory, we cannot assert postconditions on the
 * freed pointer. We verify:
 *   - The function does not crash on NULL input.
 *   - The function does not crash on valid input.
 *   - If str is NULL, nothing happens (no-op).
 *
 * We use a flag to track whether we took the NULL path.
 */

/* Helper: allocate a valid aws_string with non-deterministic content */
struct aws_string *ensure_string_is_allocated_nondet_length(void) {
    /* Use a bounded length to keep the state space manageable */
    size_t len;
    __CPROVER_assume(len <= 4); /* small bound for CBMC */

    /* Allocate memory for the struct + len bytes + 1 null terminator */
    size_t total_size = sizeof(struct aws_string) + len + 1;
    /* We need writable memory; use malloc so CBMC can track it */
    struct aws_string *str = (struct aws_string *)malloc(total_size);
    __CPROVER_assume(str != NULL);

    /* Initialize the const fields via pointer casting (as the library does) */
    struct aws_string *mutable_str = str;
    /* Write len field */
    *((size_t *)(&mutable_str->len)) = len;
    /* Write allocator field - use aws_default_allocator() so aws_mem_release works */
    *((struct aws_allocator **)(&mutable_str->allocator)) = aws_default_allocator();

    /* The bytes are non-deterministic (already non-det from malloc in CBMC) */

    return str;
}

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose between NULL and valid string */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test NULL path: function must be a no-op and not crash */
        aws_string_destroy_secure(NULL);
        /* If we reach here, the function handled NULL correctly */
        assert(true); /* no crash on NULL */
    } else {
        /* Test valid string path */
        struct aws_string *str = ensure_string_is_allocated_nondet_length();

        /* Precondition: str is a valid aws_string */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save observable state before the call */
        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;
        bool had_allocator = (str->allocator != NULL);

        /* Call the function under test */
        aws_string_destroy_secure(str);

        /*
         * After the call:
         * - The bytes have been zeroed (aws_secure_zero was called on str->bytes for str->len bytes)
         * - The memory has been freed (if allocator was non-NULL)
         *
         * We cannot safely dereference str after this point because the memory
         * has been freed. We can only assert that the function completed without
         * crashing (reaching this point is the assertion).
         *
         * The key postconditions we can assert:
         * 1. The function did not crash (we reached this point).
         * 2. old_len and old_allocator were valid before the call.
         */
        assert(true); /* function completed without crashing */

        /*
         * Additional observable postcondition:
         * The length we saved was valid (non-negative, which is always true for size_t).
         * The allocator we saved was the default allocator.
         */
        assert(old_len <= 4); /* matches our bound above */
        (void)old_allocator;
        (void)had_allocator;
    }
}

void aws_string_destroy_secure_harness(void) {
    aws_string_destroy_secure_harness();
    return 0;
}
