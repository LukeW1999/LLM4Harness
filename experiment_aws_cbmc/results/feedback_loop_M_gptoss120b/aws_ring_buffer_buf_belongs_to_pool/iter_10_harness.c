#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

/* Forward declaration for nondeterministic uint64 generator */
extern uint64_t nondet_uint64_t(void);

/* Helper to initialize an aws_ring_buffer with bounded allocation */
static void initialize_ring_buffer(struct aws_ring_buffer *ring) {
    size_t size = (size_t)nondet_uint64_t();
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);
    /* Require power‑of‑two size for the ring buffer implementation */
    __CPROVER_assume((size & (size - 1)) == 0);

    struct aws_allocator *allocator = aws_default_allocator();
    ring->allocator = allocator;
    ring->allocation = (uint8_t *)aws_mem_acquire(allocator, size);
    __CPROVER_assume(ring->allocation != NULL);
    ring->allocation_end = ring->allocation + size;
    ring->size = size;

    /* Initialize atomic head and tail to zero */
    aws_atomic_store_int(&ring->head, 0);
    aws_atomic_store_int(&ring->tail, 0);

    /* Set remaining fields required for validity */
    ring->mask = size - 1;
    ring->read_cursor = 0;
    ring->write_cursor = 0;

    /* Assume the ring buffer satisfies its validity predicate */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring));
}

/* Harness for aws_ring_buffer_buf_belongs_to_pool */
void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring;
    initialize_ring_buffer(&ring);

    /* Save old values of ring buffer fields */
    struct aws_allocator *old_allocator = ring.allocator;
    uint8_t *old_allocation = ring.allocation;
    uint8_t *old_allocation_end = ring.allocation_end;
    size_t old_size = ring.size;
    size_t old_mask = ring.mask;
    size_t old_read_cursor = ring.read_cursor;
    size_t old_write_cursor = ring.write_cursor;
    int old_head = aws_atomic_load_int(&ring.head);
    int old_tail = aws_atomic_load_int(&ring.tail);

    /* 2. Declare and bound the byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.len <= buf.capacity);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Save old values of byte buffer fields */
    uint8_t *old_buf_ptr = buf.buffer;
    size_t old_buf_len = buf.len;
    size_t old_buf_capacity = buf.capacity;
    struct aws_allocator *old_buf_allocator = buf.allocator;

    /* 3. Call the function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 4. Post‑condition: result reflects membership in the ring buffer's allocation */
    if (result) {
        assert(buf.buffer >= ring.allocation);
        assert((uint8_t *)buf.buffer + buf.capacity <= ring.allocation_end);
    } else {
        assert(!((buf.buffer >= ring.allocation) &&
                 ((uint8_t *)buf.buffer + buf.capacity <= ring.allocation_end)));
    }

    /* 5. Unchanged fields – ring buffer */
    assert(ring.allocator == old_allocator);
    assert(ring.allocation == old_allocation);
    assert(ring.allocation_end == old_allocation_end);
    assert(ring.size == old_size);
    assert(ring.mask == old_mask);
    assert(ring.read_cursor == old_read_cursor);
    assert(ring.write_cursor == old_write_cursor);
    assert(aws_atomic_load_int(&ring.head) == old_head);
    assert(aws_atomic_load_int(&ring.tail) == old_tail);

    /* 6. Unchanged fields – byte buffer */
    assert(buf.buffer == old_buf_ptr);
    assert(buf.len == old_buf_len);
    assert(buf.capacity == old_buf_capacity);
    assert(buf.allocator == old_buf_allocator);

    /* 7. Validity invariants */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
