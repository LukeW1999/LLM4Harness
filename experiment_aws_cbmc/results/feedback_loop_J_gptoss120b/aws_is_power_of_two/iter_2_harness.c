#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_is_power_of_two_harness(void) {
    size_t n = nondet_size_t();

    bool result = aws_is_power_of_two(n);

    bool expected = (n != 0) && ((n & (n - 1)) == 0);

    assert(result == expected);
}
