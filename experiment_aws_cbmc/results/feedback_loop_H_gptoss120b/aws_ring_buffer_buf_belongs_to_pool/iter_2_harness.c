#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_BUFFER_SIZE 256

int main(void) {
    /* 1. Allocate and initialise a ring buffer */
    struct aws_ring_buffer ring;
    ring.allocator = nondet_voidp();
    ring.allocation = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(ring.allocation != NULL);
    ring.allocation_end = ring.allocation + MAX_BUFFER_SIZE;
    aws_atomic_store_int(&ring.head, 0);
    aws_atomic_store_int(&ring.tail, 0);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring));

    /* 2. Allocate and initialise a byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 3. Nondeterministically decide whether the byte buffer points into the ring's allocation */
    bool belongs = __CPROVER_nondet_bool();
    if (belongs) {
        buf.buffer = ring.allocation;               /* points to start of allocation */
        buf.capacity = MAX_BUFFER_SIZE;
    } else {
        buf.buffer = ring.allocation_end + 1;       /* points outside allocation */
        buf.capacity = __CPROVER_nondet_uint();
        __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    }
    buf.len = __CPROVER_nondet_uint();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = ring.allocator;
    buf.owns_buffer = false;

    /* 4. Save old state */
    struct aws_ring_buffer old_ring = ring;
    struct aws_byte_buf old_buf = buf;

    /* 5. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring, &buf);

    /* 6. Post‑condition checks – function must be pure */
    assert(ring.allocator == old_ring.allocator);
    assert(ring.allocation == old_ring.allocation);
    assert(ring.allocation_end == old_ring.allocation_end);
    assert(aws_atomic_load_int(&ring.head) == aws_atomic_load_int(&old_ring.head));
    assert(aws_atomic_load_int(&ring.tail) == aws_atomic_load_int(&old_ring.tail));

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.owns_buffer == old_buf.owns_buffer);

    /* 7. Invariants must still hold */
    assert(aws_ring_buffer_is_valid(&ring));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* 8. Result must be consistent with the setup */
    if (belongs) {
        assert(result);
    } else {
        assert(!result);
    }

    return 0;
}
