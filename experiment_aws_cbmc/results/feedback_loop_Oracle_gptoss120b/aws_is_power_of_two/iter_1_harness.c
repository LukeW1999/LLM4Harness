#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic generators */
size_t nondet_size_t(void);
uint64_t nondet_uint64_t(void);

void aws_is_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t x = nondet_size_t();

    /* frame: allocate a dummy buffer and capture its initial state */
    size_t buf[10];
    size_t buf_old[10];
    for (size_t i = 0; i < 10; ++i) {
        buf[i] = (size_t)nondet_uint64_t();
        buf_old[i] = buf[i];
    }

    /* call the function under test */
    bool result = aws_is_power_of_two(x);

    /* postcondition: result matches the mathematical definition */
    bool expected = (x != 0) && ((x & (x - 1)) == 0);
    assert(result == expected);

    /* frame condition: no memory outside the function is modified */
    for (size_t i = 0; i < 10; ++i) {
        assert(buf[i] == buf_old[i]);
    }

    return 0;
}
