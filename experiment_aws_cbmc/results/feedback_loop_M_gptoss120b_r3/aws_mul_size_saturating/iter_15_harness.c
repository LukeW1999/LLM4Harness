#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result;
    int rc = aws_mul_size_saturating(a, b, &result);

    size_t max = (size_t)-1;
    int overflow = (b != 0 && a > max / b);

    __CPROVER_assert((rc == 0) == !overflow, "rc matches overflow condition");

    if (rc == 0) {
        __CPROVER_assert(result == a * b, "result equals product");
    }
}
