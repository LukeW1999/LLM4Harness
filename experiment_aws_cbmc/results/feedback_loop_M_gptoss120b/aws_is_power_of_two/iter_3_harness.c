#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness(void) {
    size_t n = nondet_size_t();
    size_t result = aws_round_up_to_power_of_two(n);
    (void)result;
}
