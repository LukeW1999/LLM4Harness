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

    /* Choose a non‑zero ring size within a reasonable bound */
    size_t ring_size = (size_t)nondet_uint64_t();
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);

    /* Initialize a valid ring buffer */
    int init_res = aws_ring_buffer_init(&ring_buf, allocator, ring_size);
    __CPROVER_assume(init_res == 0); /* AWS_OP_SUCCESS == 0 */

    /* Choose an offset strictly inside the allocation */
    size_t offset = (size_t)nondet_uint64_t();
    __CPROVER_assume(offset < ring_size);
    buf.buffer = ring_buf.allocation + offset;

    /* Capacity is the remaining space from the offset to the end of the allocation */
    size_t max_len = (size_t)(ring_buf.allocation_end - buf.buffer);
    __CPROVER_assume(max_len > 0);
    buf.capacity = max_len;

    /* Length must be within capacity */
    buf.len = (size_t)nondet_uint64_t();
    __CPROVER_assume(buf.len <= buf.capacity);

    buf.allocator = allocator;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);
    (void)result;
}
