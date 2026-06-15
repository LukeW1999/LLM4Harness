#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <aws/common/common.h>
#include <aws/common/allocator.h>
#include <aws/common/atomics.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();

    bool do_init = __CPROVER_nondet_bool();

    if (do_init) {
        size_t size = __CPROVER_nondet_size_t();
        __CPROVER_assume(size > 0);
        int init_result = aws_ring_buffer_init(&ring_buf, allocator, size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
        __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        memset(&ring_buf, 0, sizeof(ring_buf));
    }

    aws_ring_buffer_clean_up(&ring_buf);

    assert(aws_ring_buffer_is_valid(&ring_buf));
}
