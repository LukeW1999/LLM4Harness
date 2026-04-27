#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and bound data structures */
    struct aws_ring_buffer ring_buf;
    make_and_bound_ring_buffer(&ring_buf);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_ring_buffer old = ring_buf;

    /* 3. Call function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 4. Assert postconditions for BOTH success and failure paths */
    /* Success path: allocation should be released and structure zeroed out */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
    assert(ring_buf.head.atomics == 0);
    assert(ring_buf.tail.atomics == 0);

    /* 5. Assert fields that must NOT change regardless of result */
    /* None in this case as all fields are expected to be zeroed out */

    /* 6. Assert validity invariant always holds */
    assert(aws_ring_buffer_is_valid(&ring_buf));
}
