#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_ring_buffer_buf_belongs_to_pool_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    /* nondet size for the ring buffer allocation */
    size_t ring_size;
    __CPROVER_assume(ring_size > 0);
    __CPROVER_assume(ring_size < (size_t)1 << 30); /* reasonable upper bound */

    /* initialize ring buffer */
    int init_res = aws_ring_buffer_init(&ring_buf, alloc, ring_size);
    __CPROVER_assume(init_res == AWS_OP_SUCCESS);

    /* nondet capacity for the byte buffer */
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity < (size_t)1 << 30);

    /* initialize byte buffer */
    int buf_init_res = aws_byte_buf_init(&buf, alloc, buf_capacity);
    __CPROVER_assume(buf_init_res == AWS_OP_SUCCESS);

    /* structural validity assumptions */
    __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buf));
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save copies for frame condition checks */
    struct aws_ring_buffer ring_buf_old = ring_buf;
    struct aws_byte_buf buf_old = buf;

    /* call the function under verification */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buf, &buf);

    /* --------------------------------------------------------------------
     * Postcondition 1: return value matches the logical belonging predicate
     * -------------------------------------------------------------------- */
    bool expected = false;
    if (buf.buffer != NULL) {
        uint8_t *alloc_start = ring_buf.allocation;
        uint8_t *alloc_end   = ring_buf.allocation_end;
        expected = (buf.buffer >= alloc_start) &&
                   (buf.buffer + buf.capacity <= alloc_end);
    }
    assert(result == expected);

    /* --------------------------------------------------------------------
     * Postcondition 2: structural validity of inputs is preserved
     * -------------------------------------------------------------------- */
    assert(aws_ring_buffer_is_valid(&ring_buf));
    assert(aws_byte_buf_is_valid(&buf));

    /* --------------------------------------------------------------------
     * Postcondition 3: frame condition – no memory outside the contract is modified
     * -------------------------------------------------------------------- */
    assert(memcmp(&ring_buf, &ring_buf_old, sizeof(ring_buf)) == 0);
    assert(memcmp(&buf, &buf_old, sizeof(buf)) == 0);

    return 0;
}
