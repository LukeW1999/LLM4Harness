#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "aws/common/ring_buffer.h"
#include "aws/common/allocator.h"
#include "proof_helpers/make_common_data_structures.h"

#define MAX_ALLOC_SIZE 1024
#define ACQUIRE_SIZE   64

void aws_ring_buffer_release_harness(void) {
    /* allocator */
    struct aws_allocator *alloc = aws_default_allocator();

    /* ring buffer allocation */
    uint8_t *allocation = malloc(MAX_ALLOC_SIZE);
    __CPROVER_assume(allocation != NULL);
    struct aws_ring_buffer ring;
    aws_ring_buffer_init(&ring, alloc, allocation, MAX_ALLOC_SIZE);

    /* acquire a slice from the ring buffer */
    struct aws_byte_buf buf;
    int acquire_rc = aws_ring_buffer_acquire(&ring, &buf, ACQUIRE_SIZE);
    __CPROVER_assume(acquire_rc == 0);
    __CPROVER_assume(buf.buffer != NULL);
    __CPROVER_assume(buf.capacity == ACQUIRE_SIZE);
    __CPROVER_assume(buf.len == 0);
    __CPROVER_assume(buf.allocator == alloc);

    /* save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf   old_buf   = buf;

    /* call function under test */
    aws_ring_buffer_release(&ring, &buf);

    /* post‑conditions for the released buffer */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(buf.allocator == NULL);

    /* ring fields that must not change */
    assert(ring.allocator   == old_ring.allocator);
    assert(ring.allocation  == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(ring.head        == old_ring.head);

    /* tail must be advanced to just past the released slice */
    assert(ring.tail == (uint8_t *)old_buf.buffer + old_buf.capacity);

    /* validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
}
