/* Contract:
Preconditions:
  - ring_buffer != NULL
  - allocator != NULL (aws_default_allocator())
  - size > 0
  - aws_ring_buffer_init succeeds (returns AWS_OP_SUCCESS)
  - buf != NULL
  - aws_ring_buffer_acquire succeeds and returns a buffer that belongs to the ring buffer
  - The acquired buffer's capacity > 0
Postconditions (validity):
  - After aws_ring_buffer_release, buf->buffer == NULL
  - buf->capacity == 0
  - buf->len == 0
  - buf->allocator == NULL
  - ring_buffer->tail points to (previous buf->buffer + previous buf->capacity)
Postconditions (frame):
  - No memory outside of ring_buffer->tail atomic variable and the aws_byte_buf struct is modified
*/

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_release_harness(void) {
    /* Allocate and initialize ring buffer */
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t init_size = nondet_uint64_t();
    __CPROVER_assume(init_size > 0);
    __CPROVER_assume(init_size <= 1024 * 1024); /* reasonable bound */

    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, init_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Acquire a buffer from the ring buffer */
    struct aws_byte_buf buf;
    size_t acquire_size = nondet_uint64_t();
    __CPROVER_assume(acquire_size > 0);
    __CPROVER_assume(acquire_size <= init_size / 2); /* ensure space */

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, acquire_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    __CPROVER_assume(buf.capacity > 0);
    __CPROVER_assume(buf.buffer != NULL);

    /* Save pre-state values for postcondition checks */
    uint8_t *expected_tail = buf.buffer + buf.capacity;
    struct aws_byte_buf buf_before = buf;
    struct aws_atomic_var tail_before = ring_buffer.tail;

    /* Call the function under verification */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postcondition checks */
    /* buf should be zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* tail atomic should now point to expected location */
    uint8_t *actual_tail = AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buffer);
    assert(actual_tail == expected_tail);

    /* No other modifications to ring buffer fields */
    assert(ring_buffer.head == tail_before); /* head unchanged */
    assert(ring_buffer.allocation == ring_buffer.allocation); /* unchanged */
    assert(ring_buffer.allocation_end == ring_buffer.allocation_end); /* unchanged */
    assert(ring_buffer.allocator == allocator); /* unchanged */

    return 0;
}
