#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024U

static bool is_power_of_two(size_t x) {
    return x != 0 && (x & (x - 1)) == 0;
}

/* Helper to read the atomic pointer values (implementation‑specific) */
static uint8_t *atomic_head_ptr(const struct aws_ring_buffer *ring_buf) {
    uint8_t *ptr;
    AWS_ATOMIC_LOAD_HEAD_PTR((struct aws_ring_buffer *)ring_buf, ptr);
    return ptr;
}
static uint8_t *atomic_tail_ptr(const struct aws_ring_buffer *ring_buf) {
    uint8_t *ptr;
    AWS_ATOMIC_LOAD_TAIL_PTR((struct aws_ring_buffer *)ring_buf, ptr);
    return ptr;
}

void aws_ring_buffer_acquire_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(is_power_of_two(capacity));

    int init_res = aws_ring_buffer_init(&ring_buf, allocator, capacity);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Declare the destination byte buffer and make it initially valid */
    struct aws_byte_buf dest;
    uint8_t *dummy_buf = (uint8_t *)malloc(1);
    __CPROVER_assume(dummy_buf != NULL);
    dest.buffer = dummy_buf;
    dest.capacity = 1;
    dest.len = 0;
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 3. Nondeterministic requested size, bounded and guaranteed to succeed */
    size_t requested_size = nondet_size_t();
    __CPROVER_assume(requested_size > 0 && requested_size <= capacity - 1);

    /* 4. Save old state for immutability checks */
    struct aws_ring_buffer old_ring = ring_buf;
    uint8_t *old_head = atomic_head_ptr(&old_ring);
    uint8_t *old_tail = atomic_tail_ptr(&old_ring);
    struct aws_byte_buf old_dest = dest;

    /* 5. Call the function under test */
    int result = aws_ring_buffer_acquire(&ring_buf, requested_size, &dest);

    /* 6. Post‑condition checks */
    assert(aws_ring_buffer_is_valid(&ring_buf));

    if (result == AWS_OP_SUCCESS) {
        /* The returned buffer must lie inside the ring allocation */
        assert(dest.buffer >= ring_buf.allocation);
        assert(dest.buffer + requested_size <= ring_buf.allocation_end);
        /* The capacity of the returned buffer must equal the requested size */
        assert(dest.capacity == requested_size);
        /* The length of the returned buffer is initially zero */
        assert(dest.len == 0);

        /* New head must still be within the allocation bounds */
        uint8_t *new_head = atomic_head_ptr(&ring_buf);
        assert(new_head >= ring_buf.allocation);
        assert(new_head <= ring_buf.allocation_end);
        /* The distance moved (modulo the buffer size) must equal requested_size */
        size_t ring_size = (size_t)(ring_buf.allocation_end - ring_buf.allocation);
        size_t moved = (size_t)(new_head - old_head);
        if (new_head < old_head) {
            moved = (size_t)(ring_buf.allocation_end - old_head) + (size_t)(new_head - ring_buf.allocation);
        }
        assert(moved == requested_size);
        /* Tail may stay unchanged or be equal to the allocation start after a wrap */
        uint8_t *new_tail = atomic_tail_ptr(&ring_buf);
        (void)new_tail; /* no strict requirement */
    } else {
        /* On failure the ring buffer must be unchanged */
        assert(atomic_head_ptr(&ring_buf) == old_head);
        assert(atomic_tail_ptr(&ring_buf) == old_tail);
        assert(ring_buf.allocation == old_ring.allocation);
        assert(ring_buf.allocation_end == old_ring.allocation_end);
        assert(ring_buf.allocator == old_ring.allocator);
    }

    /* Fields that never change regardless of outcome */
    assert(ring_buf.allocator == old_ring.allocator);
    assert(ring_buf.allocation == old_ring.allocation);
    assert(ring_buf.allocation_end == old_ring.allocation_end);
}
