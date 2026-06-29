#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_reset_harness(void) {
    /* Create a valid aws_byte_buf */
    struct aws_byte_buf buf;
    
    /* Use nondet capacity bounded for CBMC tractability */
    size_t capacity;
    __CPROVER_assume(capacity <= 64);
    
    /* Initialize the buffer with a valid allocator and capacity */
    struct aws_allocator *allocator = aws_default_allocator();
    
    if (capacity == 0) {
        /* Zero capacity buffer */
        buf.buffer = NULL;
        buf.len = 0;
        buf.capacity = 0;
        buf.allocator = allocator;
    } else {
        /* Allocate buffer memory */
        buf.buffer = malloc(capacity);
        __CPROVER_assume(buf.buffer != NULL);
        buf.capacity = capacity;
        /* len must be <= capacity for a valid buffer */
        size_t initial_len;
        __CPROVER_assume(initial_len <= capacity);
        buf.len = initial_len;
        buf.allocator = allocator;
    }
    
    /* Ensure the buffer is valid before calling the function */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    
    /* Save state before the call */
    size_t old_capacity = buf.capacity;
    uint8_t *old_buffer = buf.buffer;
    struct aws_allocator *old_allocator = buf.allocator;
    
    /* Nondeterministically choose whether to zero contents */
    bool zero_contents;
    
    /* Call the function under test */
    aws_byte_buf_reset(&buf, zero_contents);
    
    /* Postcondition 1: The buffer length must be 0 after reset */
    assert(buf.len == 0);
    
    /* Postcondition 2: The capacity must remain unchanged */
    assert(buf.capacity == old_capacity);
    
    /* Postcondition 3: The buffer pointer must remain unchanged */
    assert(buf.buffer == old_buffer);
    
    /* Postcondition 4: The allocator must remain unchanged */
    assert(buf.allocator == old_allocator);
    
    /* Postcondition 5: The buffer must still be valid after reset */
    assert(aws_byte_buf_is_valid(&buf));
    
    /* Postcondition 6: If zero_contents was true and buffer is non-NULL,
     * all bytes in the buffer should be zero (up to capacity) */
    if (zero_contents && buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}

void aws_byte_buf_reset_harness(void) {
    aws_byte_buf_reset_harness();
    return 0;
}
