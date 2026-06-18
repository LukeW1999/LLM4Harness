#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare the ring buffer and set up a nondeterministic but valid state */
    struct aws_ring_buffer ring_buf;
    ring_buf.allocator = aws_default_allocator();

    /* Assume the ring buffer is in a valid state before clean up */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save the old state for later comparison */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑condition: all fields must be zero after clean up */
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);

    /* Verify that every byte of the structure is zeroed */
    for (size_t i = 0; i < sizeof(ring_buf); ++i) {
        assert(((uint8_t *)&ring_buf)[i] == 0);
    }

    /* 5. No fields should retain their old values (the whole struct is cleared) */
    (void)old; /* suppress unused variable warning */
}
