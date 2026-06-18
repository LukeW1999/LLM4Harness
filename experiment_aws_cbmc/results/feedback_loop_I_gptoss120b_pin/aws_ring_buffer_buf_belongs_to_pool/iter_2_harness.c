#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* Declare data structures */
    struct aws_ring_buffer ring_buf = {0};
    struct aws_byte_buf buf = {0};

    /* Allocate and bound the ring buffer's internal allocation */
    size_t ring_size = nondet_size_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + ring_size;

    /* Set allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    /* Initialize head and tail atomics within bounds */
    size_t head = nondet_size_t();
    __CPROVER_assume(head <= ring_size);
    size_t tail = nondet_size_t();
    __CPROVER_assume(tail <= ring_size);
    ring_buf.head = (struct aws_atomic_var){ .value = head };
    ring_buf.tail = (struct aws_atomic_var){ .value = tail };

    /* Initialize byte buffer to point inside the ring buffer allocation */
    size_t offset = nondet_size_t();
    __CPROVER_assume(offset <= ring_size);
    buf.buffer = ring_buf.allocation + offset;
    size_t max_len = (size_t)(ring_buf.allocation_end - buf.buffer);
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= max_len);
    buf.capacity = max_len;
    buf.allocator = allocator;

    /* Call the function under verification */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);
    (void)result; /* suppress unused variable warning */
}
