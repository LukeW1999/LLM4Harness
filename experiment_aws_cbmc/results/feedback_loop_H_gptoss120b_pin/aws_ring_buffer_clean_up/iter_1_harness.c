#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

void aws_ring_buffer_clean_up_harness(void) {
    /* 1. Declare and bound the ring buffer */
    struct aws_ring_buffer ring_buf;
    /* Use the default allocator */
    ring_buf.allocator = aws_default_allocator();

    /* Non-deterministically allocate the internal buffer */
    size_t alloc_size = nondet_size_t();
    __CPROVER_assume(alloc_size <= MAX_BUFFER_SIZE);
    ring_buf.allocation = alloc_size ? malloc(alloc_size) : NULL;
    ring_buf.allocation_end = ring_buf.allocation ? ring_buf.allocation + alloc_size : NULL;

    /* Non-deterministically initialize atomic variables (their exact layout is opaque) */
    /* The validity predicate will constrain them appropriately */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Save old state */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Postconditions: after clean_up all fields must be zero / NULL */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    /* The atomic variables are part of the struct; after zeroing they must be zero.
       Since their internal representation is opaque, we conservatively check that the
       whole struct matches a zero-initialized instance. */
    {
        struct aws_ring_buffer zeroed;
        AWS_ZERO_STRUCT(zeroed);
        assert(ring_buf.head.value == zeroed.head.value);
        assert(ring_buf.tail.value == zeroed.tail.value);
    }

    /* 5. Validity invariant must still hold (empty buffer is valid) */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
