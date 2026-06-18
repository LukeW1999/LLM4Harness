#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate a result buffer with guard bytes before and after */
    struct {
        uint8_t pre[8];
        size_t result;
        uint8_t post[8];
    } *buf = malloc(sizeof(*buf));
    __CPROVER_assume(buf != NULL);

    /* initialize guard bytes with nondeterministic values */
    for (size_t i = 0; i < 8; ++i) {
        buf->pre[i] = nondet_uint8_t();
        buf->post[i] = nondet_uint8_t();
    }

    /* store an initial value for the result location */
    size_t old_result = nondet_size_t();
    buf->result = old_result;

    /* capture the original guard bytes */
    uint8_t pre_orig[8];
    uint8_t post_orig[8];
    for (size_t i = 0; i < 8; ++i) {
        pre_orig[i] = buf->pre[i];
        post_orig[i] = buf->post[i];
    }

    /* call the function under test */
    int ret = aws_mul_size_checked(a, b, &buf->result);

    /* post‑condition: return‑code correctness */
    if (ret == AWS_OP_SUCCESS) {
        /* no overflow occurred */
        assert(b == 0 || a <= SIZE_MAX / b);
        /* result is the exact product */
        assert(buf->result == a * b);
    } else {
        /* overflow must have occurred */
        assert(!(b == 0 || a <= SIZE_MAX / b));
        /* result must be left unchanged on error */
        assert(buf->result == old_result);
    }

    /* frame condition: guard bytes must be unchanged */
    for (size_t i = 0; i < 8; ++i) {
        assert(buf->pre[i] == pre_orig[i]);
        assert(buf->post[i] == post_orig[i]);
    }

    free(buf);
    return 0;
}
