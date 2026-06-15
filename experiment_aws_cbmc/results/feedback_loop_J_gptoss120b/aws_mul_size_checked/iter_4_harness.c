#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Bound the inputs to keep CBMC tractable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result_storage;
    size_t *result = &result_storage;
    *result = nondet_size_t();

    size_t old_a = a;
    size_t old_b = b;
    size_t old_result = *result;
    size_t *old_result_ptr = result;

    int rc = aws_mul_size_checked(a, b, result);

    if (rc == AWS_OP_SUCCESS) {
        __CPROVER_assert(a == 0 || b <= SIZE_MAX / a, "no overflow");
        __CPROVER_assert(*result == a * b, "correct product");
    } else {
        __CPROVER_assert(*result == old_result, "result unchanged on failure");
    }

    __CPROVER_assert(a == old_a, "a unchanged");
    __CPROVER_assert(b == old_b, "b unchanged");
    __CPROVER_assert(result == old_result_ptr, "result pointer unchanged");
}
