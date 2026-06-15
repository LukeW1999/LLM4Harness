#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

/* nondeterministic generators for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_add_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate a dummy buffer to capture any unintended memory writes */
    const size_t buf_len = 10;
    uint8_t *buf = malloc(buf_len);
    __CPROVER_assume(buf != NULL);
    for (size_t i = 0; i < buf_len; ++i) {
        buf[i] = nondet_uint8_t();
    }

    /* compute a simple checksum of the buffer before the call */
    uint64_t pre_hash = 0;
    for (size_t i = 0; i < buf_len; ++i) {
        pre_hash = pre_hash * 31 + buf[i];
    }

    /* call the function under test */
    size_t result = aws_add_size_saturating(a, b);

    /* post‑condition: result correctness */
    if (a > SIZE_MAX - b) {
        __CPROVER_assert(result == SIZE_MAX,
                         "aws_add_size_saturating: overflow should yield SIZE_MAX");
    } else {
        __CPROVER_assert(result == a + b,
                         "aws_add_size_saturating: non‑overflow should yield exact sum");
    }

    /* post‑condition: no memory outside the function's contract is modified */
    uint64_t post_hash = 0;
    for (size_t i = 0; i < buf_len; ++i) {
        post_hash = post_hash * 31 + buf[i];
    }
    __CPROVER_assert(pre_hash == post_hash,
                     "aws_add_size_saturating: function must not modify unrelated memory");

    free(buf);
    return 0;
}
