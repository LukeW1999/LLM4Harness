#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

bool nondet_bool(void);
size_t nondet_size_t(void);

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    AWS_ZERO_STRUCT(ring_buf);

    struct aws_allocator *allocator = aws_default_allocator();
    ring_buf.allocator = allocator;

    bool has_allocation = nondet_bool();
    if (has_allocation) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
        uint8_t buffer_storage[MAX_BUFFER_SIZE];
        ring_buf.allocation = buffer_storage;
        ring_buf.allocation_end = buffer_storage + size;
        ring_buf.size = size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
        ring_buf.size = 0;
    }

    struct aws_allocator *old_allocator = ring_buf.allocator;

    aws_ring_buffer_clean_up(&ring_buf);

    assert(ring_buf.allocator == old_allocator);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.size == 0);
    assert(aws_atomic_load_int(&ring_buf.head) == 0);
    assert(aws_atomic_load_int(&ring_buf.tail) == 0);
}
