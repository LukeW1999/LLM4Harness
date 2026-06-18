#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_is_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t x = nondet_size_t();

    /* frame: allocate a dummy buffer and snapshot its contents */
    size_t dummy[8];
    for (size_t i = 0; i < 8; ++i) {
        dummy[i] = nondet_size_t();
    }
    size_t dummy_snapshot[8];
    memcpy(dummy_snapshot, dummy, sizeof(dummy));

    /* call the function under test */
    bool result = aws_is_power_of_two(x);

    /* postcondition: result matches the mathematical definition */
    bool expected = (x != 0) && ((x & (x - 1)) == 0);
    assert(result == expected);

    /* frame condition: no memory other than locals was modified */
    for (size_t i = 0; i < 8; ++i) {
        assert(dummy[i] == dummy_snapshot[i]);
    }

    return 0;
}
