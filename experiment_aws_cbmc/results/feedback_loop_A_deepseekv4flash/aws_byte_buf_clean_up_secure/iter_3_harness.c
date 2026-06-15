#include <aws/common/byte_buf.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *old_allocator;

    // Non-deterministically choose whether the buffer has allocated memory
    bool has_allocator = nondet_bool();

    if (has_allocator) {
        // Allocate buffer memory
        buf.buffer = (uint8_t *)malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(buf.buffer != NULL);
        buf.capacity = MAX_BUFFER_SIZE;
        buf.len = nondet_size_t();
        __CPROVER_assume(buf.len <= buf.capacity);

        // Use a non-deterministic but non-null allocator
        buf.allocator = (struct aws_allocator *)nondet_voidp();
        __CPROVER_assume(buf.allocator != NULL);
    } else {
        // Null buffer with no allocator
        buf.buffer = NULL;
        buf.len = 0;
        buf.capacity = 0;
        buf.allocator = NULL;
    }

    old_allocator = buf.allocator;

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(buf.allocator == old_allocator);

    /* If there was an allocated buffer, it should have been freed */
    // No direct assertion possible, but we ensure the pointer is NULL
}
