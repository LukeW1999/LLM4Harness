#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

/* Nondeterministic helpers (CBMC built‑ins) */
static bool nondet_bool(void) {
    bool b;
    return b;
}
static size_t nondet_size_t(void) {
    size_t s;
    return s;
}

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    __CPROVER_assume(ring_buf != NULL);

    bool do_init = nondet_bool();

    if (do_init) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size > 0);
        int init_result = aws_ring_buffer_init(ring_buf, aws_default_allocator(), size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    } else {
        ring_buf->allocation = NULL;
        ring_buf->allocation_end = NULL;
        ring_buf->allocation_size = 0;
        ring_buf->allocator = aws_default_allocator();
        aws_atomic_init_int(&ring_buf->head, 0);
        aws_atomic_init_int(&ring_buf->tail, 0);
    }

    /* Ensure the precondition of aws_ring_buffer_clean_up holds */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buf));

    /* Call the function under verification */
    aws_ring_buffer_clean_up(ring_buf);

    /* Post‑condition checks */
    assert(ring_buf->allocation == NULL);
    assert(ring_buf->allocation_end == NULL);
    assert(ring_buf->allocator == NULL);
    assert(aws_atomic_load_int(&ring_buf->head) == 0);
    assert(aws_atomic_load_int(&ring_buf->tail) == 0);

    free(ring_buf);
}
