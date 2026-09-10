#include <aws/common/ring_buffer.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* parameters */
    struct aws_byte_buf buf;
    struct aws_ring_buffer ring_buffer;

    size_t ring_buf_size;

    /* assumptions */
    ensure_ring_buffer_has_allocated_members(&ring_buffer, ring_buf_size);
    bool is_member = nondet_bool(); /* nondet assignment required to force true/false */
    if (is_member) {
        __CPROVER_assume(!aws_ring_buffer_is_empty(&ring_buffer));
        ensure_byte_buf_has_allocated_buffer_member_in_ring_buf(&buf, &ring_buffer);
    } else {
        ensure_byte_buf_has_allocated_buffer_member(&buf);
    }
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_ring_buffer ring_buf_old = ring_buffer;
    struct aws_byte_buf buf_old = buf;

    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* assertions */
    assert(is_member == result);
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
    assert(ring_buf_old == ring_buffer);
    assert(buf_old == buf);
}
