#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    const size_t max_size = (size_t)-1;

    __CPROVER_assert(aws_mul_size_saturating(0, 0) == 0, "zero times zero is zero");
    __CPROVER_assert(aws_mul_size_saturating(0, max_size) == 0, "left zero returns zero");
    __CPROVER_assert(aws_mul_size_saturating(max_size, 0) == 0, "right zero returns zero");

    __CPROVER_assert(aws_mul_size_saturating(1, max_size) == max_size, "one times max returns max");
    __CPROVER_assert(aws_mul_size_saturating(max_size, 1) == max_size, "max times one returns max");

    __CPROVER_assert(aws_mul_size_saturating(2, 3) == 6, "small non-overflow multiplication returns product");
    __CPROVER_assert(
        aws_mul_size_saturating(max_size, 2) == max_size, "overflow multiplication saturates to max size");
    __CPROVER_assert(
        aws_mul_size_saturating(2, max_size) == max_size, "overflow multiplication saturates to max size");
}
