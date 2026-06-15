#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <aws/common/atomics.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

/* Helper to nondeterministically decide whether to initialize the ring buffer */
static bool nondet_bool(void);
static size_t nondet_size_t(void);

void aws_ring_buffer_clean_up_harness(void) {
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    __CPROVER_assume(ring_buf != NULL);

    /* Choose whether to create a fully initialized ring buffer or leave it empty */
    bool do_init = nondet_bool();

    if (do_init) {
        size_t size = nondet_size_t();
        __CPROVER_assume(size > 0);
        int init_result = aws_ring_buffer_init(ring_buf, aws_default_allocator(), size);
        __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    } else {
        /* Manually construct a ring buffer with a NULL allocation */
        ring_buf->allocation = NULL;
        ring_buf->allocation_end = NULL;
        ring_buf->allocator = aws_default_allocator();
        aws_atomic_store_int(&ring_buf->head, 0);
        aws_atomic_store_int(&ring_buf->tail, 0);
    }

    /* Capture a copy of the allocator pointer for frame checking */
    struct aws_allocator *saved_allocator = ring_buf->allocator;

    /* Call the function under verification */
    aws_ring_buffer_clean_up(ring_buf);

    /* Post‑condition checks */
    assert(ring_buf->allocation == NULL);
    assert(ring_buf->allocation_end == NULL);
    assert(ring_buf->allocator == NULL);
    assert(aws_atomic_load_int(&ring_buf->head) == 0);
    assert(aws_atomic_load_int(&ring_buf->tail) == 0);

    /* Frame condition: the allocator object itself must not be modified */
    if (saved_allocator != NULL) {
        /* The default allocator is a singleton; its internal state should remain unchanged.
           Since we cannot inspect its internals here, we only assert that the pointer value
           is unchanged. */
        assert(ring_buf->allocator == saved_allocator);
    }

    free(ring_buf);
    return 0;
}

/* Nondeterministic helpers (CBMC built‑ins) */
bool nondet_bool(void) {
    bool b;
    return b;
}

size_t nondet_size_t(void) {
    size_t s;
    return s;
}
