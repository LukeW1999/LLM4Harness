#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    /* allocate ring buffer's internal memory */
    size_t capacity;
    __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);
    uint8_t *allocation = (uint8_t *)can_fail_malloc(capacity);
    __CPROVER_assume(allocation != NULL);

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;
    ring_buf.allocation = allocation;
    ring_buf.allocation_end = allocation + capacity;

    /* set up the byte buffer to be released, ensuring it is within the ring buffer's allocation */
    size_t buf_start;
    __CPROVER_assume(buf_start < capacity);
    size_t buf_cap;
    __CPROVER_assume(buf_cap > 0 && buf_cap <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf_start + buf_cap <= capacity);

    /* initialize ring buffer head and tail to make the release valid */
    ring_buf.head = allocation;
    ring_buf.tail = allocation + buf_start;

    buf.buffer = allocation + buf_start;
    buf.capacity = buf_cap;
    buf.len = 0;
    __CPROVER_assume(buf.len <= buf_cap);
    buf.allocator = allocator;

    /* call the function under verification */
    aws_ring_buffer_release(&ring_buf, &buf);
}
