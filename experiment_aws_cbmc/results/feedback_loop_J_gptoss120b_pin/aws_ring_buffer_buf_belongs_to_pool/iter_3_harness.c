#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;

    size_t ring_size;
    __CPROVER_assume(ring_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(ring_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + ring_size;

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    aws_atomic_store_int(&ring_buf.head, 0);
    aws_atomic_store_int(&ring_buf.tail, 0);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    if (result) {
        assert(buf.buffer >= ring_buf.allocation);
        assert(buf.buffer + buf.capacity <= ring_buf.allocation_end);
    } else {
        assert(!(buf.buffer >= ring_buf.allocation &&
                 buf.buffer + buf.capacity <= ring_buf.allocation_end));
    }
}
