#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);

void aws_add_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;

    size_t result;
    size_t *pResult = &result;
    size_t *old_pResult = pResult;

    bool overflow = aws_add_size_saturating(a, b, pResult);

    __CPROVER_assert(pResult == old_pResult, "pointer unchanged");
    __CPROVER_assert(a == old_a, "a unchanged");
    __CPROVER_assert(b == old_b, "b unchanged");

    if (overflow) {
        __CPROVER_assert(*pResult == SIZE_MAX, "overflow result");
        __CPROVER_assert(a > SIZE_MAX - b, "overflow condition");
    } else {
        __CPROVER_assert(*pResult == a + b, "no overflow result");
        __CPROVER_assert(a <= SIZE_MAX - b, "no overflow condition");
    }
}
