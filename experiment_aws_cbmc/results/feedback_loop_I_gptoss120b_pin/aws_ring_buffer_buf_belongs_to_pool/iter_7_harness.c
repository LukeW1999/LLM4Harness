#include <stdbool.h>
#include <stddef.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t ring_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    /* Initialize a valid ring buffer */
    int init_res = aws_ring_buffer_init(&ring_buf, allocator, ring_size);
    __CPROVER_assume(init_res == 0); /* AWS_OP_SUCCESS == 0 */

    /* Set head and tail within bounds */
    size_t head = (size_t)nondet_uint64_t();
    __CPROVER_assume(head <= ring_size);
    size_t tail = (size_t)nondet_uint64_t();
    __CPROVER_assume(tail <= ring_size);
    ring_buf.head = (struct aws_atomic_var){ .value = head };
    ring_buf.tail = (struct aws_atomic_var){ .value = tail };

    /* Create a byte buffer that points inside the ring buffer's allocation */
    size_t offset = (size_t)nondet_uint64_t();
    __CPROVER_assume(offset <= ring_size);
    buf.buffer = ring_buf.allocation + offset;

    size_t max_len = (size_t)(ring_buf.allocation_end - buf.buffer);
    buf.capacity = max_len;

    buf.len = (size_t)nondet_uint64_t();
    __CPROVER_assume(buf.len <= max_len);

    buf.allocator = allocator;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);
    (void)result;
}
