#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <aws/common/memory.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic size_t */
size_t nondet_size_t(void);

void aws_byte_buf_init_harness(void) {
    /* Allocate the byte buffer structure */
    struct aws_byte_buf *buf = malloc(sizeof *buf);
    __CPROVER_assume(buf != NULL);

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Nondeterministic capacity */
    size_t capacity = nondet_size_t();

    /* Call the function under test */
    int ret = aws_byte_buf_init(buf, allocator, capacity);

    /* General post‑condition on return code */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* Verify the validity predicate always holds */
    assert(aws_byte_buf_is_valid(buf));

    if (ret == AWS_OP_SUCCESS) {
        /* Success path invariants */
        assert(buf->len == 0);
        assert(buf->capacity == capacity);
        assert(buf->allocator == allocator);
        if (capacity == 0) {
            assert(buf->buffer == NULL);
        } else {
            assert(buf->buffer != NULL);
        }
    } else {
        /* Error path – struct must be zero‑initialized */
        assert(buf->len == 0);
        assert(buf->capacity == 0);
        assert(buf->buffer == NULL);
        assert(buf->allocator == NULL);
    }

    return 0;
}
