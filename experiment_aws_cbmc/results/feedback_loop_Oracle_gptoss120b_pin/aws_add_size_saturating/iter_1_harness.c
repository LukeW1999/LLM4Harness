#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic generators */
size_t nondet_size_t(void);
int nondet_int(void);

void aws_add_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Frame condition: allocate a dummy buffer and snapshot its contents */
    size_t dummy_buf[10];
    size_t dummy_buf_snapshot[10];
    for (size_t i = 0; i < 10; ++i) {
        dummy_buf[i] = nondet_size_t();
        dummy_buf_snapshot[i] = dummy_buf[i];
    }

    /* Call the function under test */
    size_t result = aws_add_size_saturating(a, b);

    /* Postcondition 1: return value correctness (saturation semantics) */
    if (a > SIZE_MAX - b) {
        /* overflow case */
        assert(result == SIZE_MAX);
    } else {
        /* no overflow case */
        assert(result == a + b);
    }

    /* Postcondition 2: no side‑effects on unrelated memory (frame condition) */
    for (size_t i = 0; i < 10; ++i) {
        assert(dummy_buf[i] == dummy_buf_snapshot[i]);
    }

    return 0;
}
