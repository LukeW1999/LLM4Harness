#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare the ring buffer and bound its allocation size */
    struct aws_ring_buffer ring_buf;
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);

    /* Allocate the underlying buffer (may be NULL if size is 0) */
    uint8_t *allocation = NULL;
    if (alloc_size > 0) {
        allocation = malloc(alloc_size);
        __CPROVER_assume(allocation != NULL);
    }

    /* Initialise the ring buffer fields */
    ring_buf.allocation      = allocation;
    ring_buf.allocation_end  = (allocation != NULL) ? allocation + alloc_size : NULL;
    ring_buf.allocator       = aws_default_allocator();

    /* Initialise atomic head and tail to nondeterministic but bounded values */
    ring_buf.head = (struct aws_atomic_var){ .value = nondet_uint64_t() };
    ring_buf.tail = (struct aws_atomic_var){ .value = nondet_uint64_t() };

    /* Assume the ring buffer is initially valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Post‑conditions: all fields must be zero / NULL after clean‑up */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.head.value == 0);
    assert(ring_buf.tail.value == 0);

    /* 5. No fields other than those above should retain old values */
    /* (All fields are covered by the assertions above) */

    /* 6. Validity invariant must still hold (zeroed struct is a valid empty buffer) */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
