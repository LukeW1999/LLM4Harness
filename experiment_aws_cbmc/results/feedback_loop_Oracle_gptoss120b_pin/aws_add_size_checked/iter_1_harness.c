/* CBMC harness for aws_add_size_checked */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <aws/common/math.h>
#include <aws/common/common.h>

/* Helper to obtain nondeterministic size_t values */
size_t nondet_size_t(void);
size_t nondet_uint(void);

/* Structure to check frame conditions */
struct test_buf {
    size_t dummy_before;
    size_t result;
    size_t dummy_after;
};

void aws_add_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate buffer for result and dummy fields */
    struct test_buf *buf = malloc(sizeof(*buf));
    __CPROVER_assume(buf != NULL);

    /* initialize buffer with nondeterministic values */
    buf->dummy_before = nondet_size_t();
    buf->dummy_after  = nondet_size_t();

    /* keep copies of the dummy fields for frame condition checks */
    size_t dummy_before_old = buf->dummy_before;
    size_t dummy_after_old  = buf->dummy_after;

    /* call the function under test */
    int ret = aws_add_size_checked(a, b, &buf->result);

    /* compute overflow condition for size_t */
    bool overflow = (b > 0) && (a > (SIZE_MAX - b));

    /* 1. Return value / error code correctness */
    if (overflow) {
        assert(ret == AWS_OP_ERR);
    } else {
        assert(ret == AWS_OP_SUCCESS);
        assert(buf->result == a + b);
    }

    /* 2. Output buffer length/capacity invariants (result fits in size_t) */
    if (ret == AWS_OP_SUCCESS) {
        assert(buf->result <= SIZE_MAX);
    }

    /* 3. Frame conditions: dummy fields unchanged */
    assert(buf->dummy_before == dummy_before_old);
    assert(buf->dummy_after  == dummy_after_old);

    /* clean up */
    free(buf);
    return 0;
}

/* Definitions of nondeterministic helpers for CBMC */
size_t nondet_size_t(void) {
    size_t x;
    return x;
}
size_t nondet_uint(void) {
    size_t x;
    return x;
}
