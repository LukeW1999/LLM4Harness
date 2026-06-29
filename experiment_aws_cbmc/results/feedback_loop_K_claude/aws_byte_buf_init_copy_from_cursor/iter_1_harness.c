#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the buffer size for tractability */
#define MAX_BUFFER_SIZE 16

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    assert(allocator != NULL);

    /* Set up dest buffer (uninitialized, function should zero it) */
    struct aws_byte_buf dest;

    /* Set up src cursor with nondet length and valid pointer */
    struct aws_byte_cursor src;
    size_t src_len;
    __CPROVER_assume(src_len <= MAX_BUFFER_SIZE);
    src.len = src_len;

    if (src_len > 0) {
        uint8_t *src_data = malloc(src_len);
        __CPROVER_assume(src_data != NULL);
        src.ptr = src_data;
    } else {
        /* For zero-length cursor, ptr may be NULL or non-NULL; both are valid */
        src.ptr = NULL;
    }

    /* Precondition: src must be a valid cursor */
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save a copy of src to verify it is not modified */
    struct aws_byte_cursor src_before = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions on return value */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest must be a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&dest));

        /* Length and capacity must equal src.len */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        /* Allocator must be set correctly */
        assert(dest.allocator == allocator);

        if (src.len == 0) {
            /* Zero-length: buffer should be NULL */
            assert(dest.buffer == NULL);
        } else {
            /* Non-zero length: buffer must be non-NULL and contain a copy */
            assert(dest.buffer != NULL);
            /* Verify contents were copied correctly */
            assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
        }

        /* Clean up allocated buffer */
        aws_byte_buf_clean_up(&dest);
    } else {
        /* On failure: dest should have been zeroed at start, buffer is NULL */
        /* (The function zeros dest at the beginning via AWS_ZERO_STRUCT) */
        assert(result == AWS_OP_ERR);
        /* src.len must have been > 0 for allocation to fail */
        assert(src.len > 0);
    }

    /* Postcondition: src cursor was not modified (passed by value, so this
       checks the local copy we saved before the call) */
    assert(src_before.len == src.len);
    assert(src_before.ptr == src.ptr);

    return 0;
}
