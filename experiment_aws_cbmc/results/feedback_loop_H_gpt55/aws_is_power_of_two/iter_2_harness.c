#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_is_power_of_two_harness(void) {
    size_t n = nondet_size_t();

    bool result = aws_is_power_of_two(n);

    assert(result == (n != 0 && (n & (n - 1)) == 0));
}
