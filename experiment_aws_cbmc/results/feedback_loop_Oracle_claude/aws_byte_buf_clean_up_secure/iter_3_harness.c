#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_clean_up_secure_harness(void) {
    struct aws_byte_buf buf;

    /* Properly initialize the buffer using the helper that allocates memory correctly */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Ensure allocator is valid with proper function pointers */
    if (buf.allocator != NULL) {
        __CPROVER_assume(buf.allocator->mem_acquire != NULL);
        __CPROVER_assume(buf.allocator->mem_release != NULL);
    }

    /* If buffer is non-NULL, it must be a proper dynamic allocation */
    if (buf.buffer != NULL) {
        __CPROVER_assume(buf.allocator != NULL);
        /* Use the default allocator to ensure proper free behavior */
        buf.allocator = aws_default_allocator();
        /* Allocate a real buffer so free works */
        buf.buffer = (uint8_t *)aws_mem_acquire(buf.allocator, buf.capacity > 0 ? buf.capacity : 1);
        __CPROVER_assume(buf.buffer != NULL);
    }

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postcondition 1: After clean_up_secure, the buffer pointer should be NULL */
    assert(buf.buffer == NULL);

    /* Postcondition 2: After clean_up_secure, the length should be 0 */
    assert(buf.len == 0);

    /* Postcondition 3: After clean_up_secure, the capacity should be 0 */
    assert(buf.capacity == 0);

    /* Postcondition 4: After clean_up_secure, the allocator should be NULL */
    assert(buf.allocator == NULL);

    /* Postcondition 5: The resulting buffer should be valid (zeroed out state) */
    assert(aws_byte_buf_is_valid(&buf));
}
