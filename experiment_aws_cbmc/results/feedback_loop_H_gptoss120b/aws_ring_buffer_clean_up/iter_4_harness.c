#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    size_t buf_size;
    __CPROVER_assume(buf_size > 0);
    __CPROVER_assume((buf_size & (buf_size - 1)) == 0);   /* power‑of‑two */
    __CPROVER_assume(buf_size <= 1024);                  /* bound for CBMC */

    uint8_t *buf = alloc->mem_acquire(alloc, buf_size);
    __CPROVER_assume(buf != NULL);

    ring_buf.allocator      = alloc;
    ring_buf.allocation     = buf;
    ring_buf.allocation_end = buf + buf_size;
    ring_buf.size           = buf_size;
    ring_buf.mask           = buf_size - 1;

    uint64_t head_val;
    uint64_t tail_val;
    __CPROVER_assume(head_val < buf_size);
    __CPROVER_assume(tail_val < buf_size);
    ring_buf.head = (struct aws_atomic_var){ .value = head_val };
    ring_buf.tail = (struct aws_atomic_var){ .value = tail_val };

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.size == 0);
    assert(ring_buf.mask == 0);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);
}
