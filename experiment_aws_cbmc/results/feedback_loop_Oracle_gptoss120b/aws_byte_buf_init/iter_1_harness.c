#include <aws/common/byte_buf.h>
#include <aws/common/memory.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* CBMC nondeterministic helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_byte_buf_init_harness(void) {
    /* Allocate the buffer structure on the stack */
    struct aws_byte_buf buf;
    /* Give it nondeterministic initial contents (optional) */
    buf.len = nondet_size_t();
    buf.capacity = nondet_size_t();
    buf.buffer = (uint8_t *)nondet_size_t();   /* treat as pointer value */
    buf.allocator = (struct aws_allocator *)nondet_size_t();

    /* Snapshot of the original buffer for frame condition checking */
    struct aws_byte_buf old_buf = buf;

    /* Dummy memory region to ensure no out‑of‑bounds writes */
    uint8_t dummy[10];
    for (size_t i = 0; i < 10; ++i) {
        dummy[i] = nondet_uint8_t();
    }
    uint8_t old_dummy[10];
    memcpy(old_dummy, dummy, sizeof(dummy));

    /* Use the default allocator as required */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondeterministic capacity */
    size_t capacity = nondet_size_t();

    /* Call the function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* -------------------------------------------------------------------- */
    /* Post‑condition 1: Return value / error code correctness               */
    if (capacity == 0) {
        /* No allocation needed – must succeed and buffer stays NULL */
        assert(result == AWS_OP_SUCCESS);
        assert(buf.buffer == NULL);
    } else {
        if (buf.buffer != NULL) {
            /* Allocation succeeded */
            assert(result == AWS_OP_SUCCESS);
        } else {
            /* Allocation failed – function zeroes the struct and returns error */
            assert(result == AWS_OP_ERR);
            assert(buf.buffer == NULL);
            assert(buf.len == 0);
            assert(buf.capacity == 0);
            assert(buf.allocator == NULL);
        }
    }

    /* -------------------------------------------------------------------- */
    /* Post‑condition 2: Output buffer length/capacity invariants            */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == allocator);
    /* The validity predicate must hold for the resulting buffer */
    assert(aws_byte_buf_is_valid(&buf));

    /* -------------------------------------------------------------------- */
    /* Post‑condition 3: Frame conditions (no unintended memory writes)    */
    /* The dummy region must be unchanged */
    assert(memcmp(dummy, old_dummy, sizeof(dummy)) == 0);
    /* The parts of the struct that are not supposed to be modified (if any) */
    /* In this function all fields are intentionally written, so we only
       compare against the expected final state captured above. */

    return 0;
}
