#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* allocate ring buffer memory */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size > 0);
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buf.allocation != NULL);
    ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    ring_buf.allocator = allocator;

    /* nondeterministically choose head and tail pointers within the allocation */
    size_t head_offset = nondet_size_t() % (alloc_size + 1);
    size_t tail_offset = nondet_size_t() % (alloc_size + 1);
    ring_buf.head.value = (void *)(ring_buf.allocation + head_offset);
    ring_buf.tail.value = (void *)(ring_buf.allocation + tail_offset);

    /* assume the ring buffer is valid before the call */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Prepare function arguments */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0);
    __CPROVER_assume(requested_size <= MAX_BUFFER_SIZE);

    struct aws_byte_buf dest;
    /* dest must be an uninitialized (invalid) buffer before the call */
    dest.buffer = NULL;
    dest.capacity = 0;
    dest.len = 0;
    __CPROVER_assume(!aws_byte_buf_is_valid(&dest));

    /* 3. Save old state */
    struct aws_ring_buffer old = ring_buf;
    uint8_t *old_head = (uint8_t *)old.head.value;
    uint8_t *old_tail = (uint8_t *)old.tail.value;

    /* 4. Call function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 5. Postconditions for success */
    if (result == AWS_OP_SUCCESS) {
        /* dest points inside the ring buffer allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert((uint8_t *)dest.buffer + requested_size <= ring_buf.allocation_end);
        assert(dest.capacity == requested_size);
        assert(dest.len == 0);
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator and allocation region stay the same */
        assert(ring_buf.allocator == old.allocator);
        assert(ring_buf.allocation == old.allocation);
        assert(ring_buf.allocation_end == old.allocation_end);
    } else {
        /* 6. Postconditions for failure: ring buffer unchanged */
        assert(ring_buf.allocator == old.allocator);
        assert(ring_buf.allocation == old.allocation);
        assert(ring_buf.allocation_end == old.allocation_end);
        assert((uint8_t *)ring_buf.head.value == old_head);
        assert((uint8_t *)ring_buf.tail.value == old_tail);
        /* dest remains invalid */
        assert(!aws_byte_buf_is_valid(&dest));
    }

    /* 7. Validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    /* clean up */
    free(ring_buf.allocation);
}
