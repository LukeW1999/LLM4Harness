#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare the ring buffer and assume it is a valid, bounded instance */
    struct aws_ring_buffer ring_buf;
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save the old state for later comparison */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zero after clean‑up */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* 5. The atomic variables are part of the struct; after zeroing they must be zero.
       Since the definition of `struct aws_atomic_var` is opaque here, we compare the
       whole struct to a zero‑initialised temporary. */
    {
        struct aws_atomic_var zero_atomic = { 0 };
        assert(ring_buf.head == zero_atomic);
        assert(ring_buf.tail == zero_atomic);
    }

    /* 6. The ring buffer must still satisfy its validity predicate (zeroed struct is
       considered a valid, empty ring buffer). */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
