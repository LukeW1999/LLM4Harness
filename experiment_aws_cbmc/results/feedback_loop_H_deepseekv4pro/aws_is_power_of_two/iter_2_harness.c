#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_is_power_of_two_harness() {
    size_t val = nondet_size_t();
    bool result = aws_is_power_of_two(val);
    assert(result == ((val != 0) && ((val & (val - 1)) == 0)));
}
