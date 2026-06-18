#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic generators */
extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);
extern int nondet_int(void);

void aws_add_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* buffer to hold the result and guard bytes */
    struct {
        uint8_t pre[8];
        size_t  result;
        uint8_t post[8];
    } buf;

    /* initialize guard bytes and result with nondeterministic values */
    for (size_t i = 0; i < 8; ++i) {
        buf.pre[i]  = nondet_uint8_t();
        buf.post[i] = nondet_uint8_t();
    }
    buf.result = nondet_size_t();

    /* keep copies of guard bytes for later comparison */
    uint8_t pre_copy[8];
    uint8_t post_copy[8];
    for (size_t i = 0; i < 8; ++i) {
        pre_copy[i]  = buf.pre[i];
        post_copy[i] = buf.post[i];
    }

    /* call the function under test */
    int ret = aws_add_size_checked(a, b, &buf.result);

    /* compute overflow condition for size_t */
    bool overflow = (b > 0) && (a > (SIZE_MAX - b));

    /* 1. Return value / error code correctness */
    if (ret == AWS_OP_SUCCESS) {
        assert(!overflow);
        assert(buf.result == a + b);
    } else {
        assert(ret == AWS_OP_ERR);
        assert(overflow);
    }

    /* 2. Output buffer length/capacity invariants are covered by the above checks */

    /* 3. Frame condition: guard bytes must be unchanged */
    for (size_t i = 0; i < 8; ++i) {
        assert(buf.pre[i]  == pre_copy[i]);
        assert(buf.post[i] == post_copy[i]);
    }

    return 0;
}
