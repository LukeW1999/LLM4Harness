#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_is_power_of_two_harness() {
    size_t val = nondet_size_t();
    bool result = aws_is_power_of_two(val);

    if (val != 0 && (val & (val - 1)) == 0) {
        assert(result);
    } else {
        assert(!result);
    }
}
