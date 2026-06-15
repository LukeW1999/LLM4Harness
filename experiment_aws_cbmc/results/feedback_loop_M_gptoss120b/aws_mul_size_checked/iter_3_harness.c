#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    bool result_is_null = nondet_bool();

    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result_storage;
    size_t *result_ptr = result_is_null ? NULL : &result_storage;

    bool overflow = aws_mul_size_checked(a, b, result_ptr);

    if (result_is_null) {
        assert(overflow);
    } else {
        if (overflow) {
            assert(a != 0 && b != 0 && a > SIZE_MAX / b);
        } else {
            assert(result_storage == a * b);
        }
    }
}
