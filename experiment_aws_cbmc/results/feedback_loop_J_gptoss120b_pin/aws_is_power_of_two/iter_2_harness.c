#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n = nondet_size_t();

    size_t result = nondet_size_t();
    size_t old_result = result;

    int rc = aws_round_up_to_power_of_two(n, &result);

    if (rc == AWS_OP_SUCCESS) {
        assert(result != 0);
        assert((result & (result - 1)) == 0);
        if (n == 0) {
            assert(result == 1);
        } else {
            assert(result >= n);
        }
        assert(result <= ((size_t)1 << (sizeof(size_t) * 8 - 1)));
    } else {
        assert(result == old_result);
    }
}
