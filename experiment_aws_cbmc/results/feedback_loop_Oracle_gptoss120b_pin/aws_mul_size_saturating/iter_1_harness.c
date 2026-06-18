#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_saturating_harness(void) {
    /* nondeterministic inputs */
    size_t a;
    size_t b;

    /* a and b are left uninitialized -> nondeterministic */
    (void)a;
    (void)b;

    /* snapshot of unrelated memory to check frame condition */
    size_t snapshot[8];
    size_t before[8];
    for (size_t i = 0; i < 8; ++i) {
        snapshot[i] = (size_t)malloc(1); /* allocate distinct objects */
        before[i] = snapshot[i];
    }

    /* call the function under test */
    size_t result = aws_mul_size_saturating(a, b);

    /* compute expected result */
    size_t expected;
    if (a != 0 && b > SIZE_MAX / a) {
        expected = SIZE_MAX;
    } else {
        expected = a * b;
    }

    /* postcondition: return value correctness */
    assert(result == expected);

    /* frame condition: unrelated memory must be unchanged */
    for (size_t i = 0; i < 8; ++i) {
        assert(snapshot[i] == before[i]);
    }

    /* clean up allocated dummy objects */
    for (size_t i = 0; i < 8; ++i) {
        free((void *)snapshot[i]);
    }

    return 0;
}
