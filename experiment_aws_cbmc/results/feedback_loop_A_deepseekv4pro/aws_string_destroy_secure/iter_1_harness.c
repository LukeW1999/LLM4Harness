#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

/* Maximum assumed length of the string for bounding nondet length */
#define MAX_STRING_LENGTH MAX_BUFFER_SIZE

void aws_string_destroy_secure_harness(void) {
    /* Non-deterministically choose between NULL and a valid string */
    bool is_null = nondet_bool();
    struct aws_string *str = NULL;
    const uint8_t *old_bytes = NULL;
    size_t old_len = 0;
    struct aws_allocator *old_allocator = NULL;

    if (!is_null) {
        /* Construct a valid, bounded string */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LENGTH);

        /* Allocate enough memory for header + len bytes (using len extra
         * beyond sizeof(struct) to accommodate the flexible array member) */
        size_t alloc_size = sizeof(struct aws_string) + len;
        str = (struct aws_string *)malloc(alloc_size);
        /* Assume allocation succeeds (we want to test the destroy path) */
        __CPROVER_assume(str != NULL);

        /* Set the allocator – either default (to exercise the release branch)
         * or NULL (static-like string, no deallocation) */
        str->allocator = nondet_bool() ? aws_default_allocator() : NULL;

        /* Set the length – note: const field but we are inside initialization */
        /* Use a pointer cast to bypass const for CBMC (same as real code does) */
        size_t *len_ptr = (size_t *)&str->len;
        *len_ptr = len;

        /* Fill the data bytes (up to len) with nondet values */
        uint8_t *bytes_ptr = (uint8_t *)str->bytes;
        for (size_t i = 0; i < len; ++i) {
            bytes_ptr[i] = nondet_uint8_t();
        }

        /* Ensure the constructed string passes the validity check */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save old state for postcondition checks */
        old_bytes = aws_string_bytes(str);
        old_len = len;
        old_allocator = str->allocator;
    }

    /* Call the function under verification */
    aws_string_destroy_secure(str);

    /* Postconditions: if str was non-NULL, all its data bytes (the original
     * string content, length old_len) must now be zero.  This holds regardless
     * of whether the allocator was NULL or not, because aws_secure_zero
     * executes before any conditional deallocation. */
    if (!is_null) {
        for (size_t i = 0; i < old_len; ++i) {
            assert(old_bytes[i] == 0);
        }

        /* No further checks on str itself – it may have been freed.
         * If old_allocator was NULL, the memory remains but the string is
         * effectively destroyed (its content zeroed and structure unchanged).
         * We refrain from additional assertions because accessing a freed
         * object is undefined behavior in the real world. */
    }
    /* If str was NULL, the function is a no-op – nothing to assert. */
}
