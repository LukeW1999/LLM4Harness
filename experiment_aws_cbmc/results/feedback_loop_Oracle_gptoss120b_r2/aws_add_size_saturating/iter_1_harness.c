#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_add_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* allocate a dummy buffer to check frame condition */
    uint8_t *buf = (uint8_t *)malloc(10);
    __CPROVER_assume(buf != NULL);
    uint8_t snapshot[10];
    for (size_t i = 0; i < 10; ++i) {
        buf[i] = nondet_uint8_t();
        snapshot[i] = buf[i];
    }

    /* call the function under test */
    size_t result = aws_add_size_saturating(a, b);

    /* post‑condition: result is either the saturated value or the exact sum */
    if (a > SIZE_MAX - b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a + b);
    }

    /* frame condition: the dummy buffer must be unchanged */
    for (size_t i = 0; i < 10; ++i) {
        assert(buf[i] == snapshot[i]);
    }

    free(buf);
    return 0;
}
