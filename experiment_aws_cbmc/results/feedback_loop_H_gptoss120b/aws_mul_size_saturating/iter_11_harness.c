#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <aws/common/math.h>

extern size_t nondet_size_t(void);
extern _Bool nondet_bool(void);
extern void __CPROVER_assume(_Bool condition);

int main(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    _Bool expect_overflow = nondet_bool();

    if (expect_overflow) {
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b != 0);
        __CPROVER_assume(a > SIZE_MAX / b);
    } else {
        __CPROVER_assume(b == 0 || a <= SIZE_MAX / b);
        if (b != 0) {
            __CPROVER_assume(a <= SIZE_MAX / b);
        }
    }

    size_t result = 0;
    bool overflow = aws_mul_size_saturating(a, b, &result);

    assert(overflow == (a != 0 && b != 0 && a > SIZE_MAX / b));

    if (overflow) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }

    return 0;
}
