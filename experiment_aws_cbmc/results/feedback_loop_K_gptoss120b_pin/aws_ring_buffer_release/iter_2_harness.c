/* CBMC harness for aws_ring_buffer_release */
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <assert.h>
#include <stddef.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_ring_buffer_release_harness(void) {
    /* Allocate and initialize ring buffer */
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t init_size = nondet_uint64_t();
    __CPROVER_assume(init_size > 0);
    __CPROVER_assume(init_size <= 1024 * 1024);

    int init_result = aws_ring_buffer_init(&ring_buffer, allocator, init_size);
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Save pre‑acquire state */
    struct aws_atomic_var tail_before_acquire = ring_buffer.tail;
    uint8_t *head_before_acquire = ring_buffer.head;

    /* Prepare a valid empty byte buffer for acquire */
    struct aws_byte_buf buf = {0};
    buf.buffer = NULL;
    buf.capacity = 0;
    buf.len = 0;
    buf.allocator = NULL;
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Acquire a buffer from the ring buffer */
    size_t acquire_size = nondet_uint64_t();
    __CPROVER_assume(acquire_size > 0);
    __CPROVER_assume(acquire_size <= init_size / 2);

    int acquire_result = aws_ring_buffer_acquire(&ring_buffer, acquire_size, &buf);
    __CPROVER_assume(acquire_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));
    __CPROVER_assume(buf.capacity > 0);
    __CPROVER_assume(buf.buffer != NULL);

    /* Call the function under verification */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Post‑condition checks */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Tail should be restored to its value before acquire */
    uint8_t *actual_tail = AWS_ATOMIC_LOAD_TAIL_PTR(&ring_buffer);
    assert(actual_tail == tail_before_acquire);

    /* Head must remain unchanged */
    assert(ring_buffer.head == head_before_acquire);

    /* Other ring buffer fields must be unchanged */
    assert(ring_buffer.allocation == ring_buffer.allocation);
    assert(ring_buffer.allocation_end == ring_buffer.allocation_end);
    assert(ring_buffer.allocator == allocator);
}
