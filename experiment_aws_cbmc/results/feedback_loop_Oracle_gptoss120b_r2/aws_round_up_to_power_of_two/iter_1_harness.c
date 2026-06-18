#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic generators */
size_t nondet_size_t(void);
int nondet_int(void);

void aws_round_up_to_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t n = nondet_size_t();

    /* result buffer with nondeterministic initial content */
    size_t result = nondet_size_t();
    size_t result_before = result;

    /* frame condition buffer (unrelated memory) */
    size_t frame[8];
    for (size_t i = 0; i < 8; i++) {
        frame[i] = nondet_size_t();
    }
    size_t frame_before[8];
    memcpy(frame_before, frame, sizeof(frame));

    /* call the function under verification */
    int ret = aws_round_up_to_power_of_two(n, &result);

    /* 1. Return value / error code correctness */
    assert(ret == AWS_OP_SUCCESS || ret == AWS_OP_ERR);

    /* 2. Output buffer invariants */
    if (ret == AWS_OP_SUCCESS) {
        /* result must be a power of two */
        assert(aws_is_power_of_two(result));
        /* result must be >= n (including the n==0 case where result==1) */
        assert(result >= n);
        /* result must not exceed the largest power‑of‑two representable */
        assert(result <= SIZE_MAX_POWER_OF_TWO);
    } else {
        /* on error the output must be left unchanged */
        assert(result == result_before);
    }

    /* 3. Frame condition – no other memory modified */
    for (size_t i = 0; i < 8; i++) {
        assert(frame[i] == frame_before[i]);
    }

    return 0;
}
