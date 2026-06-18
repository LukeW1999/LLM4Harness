#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic helpers */
size_t nondet_size_t(void);
uint64_t nondet_uint64_t(void);

void aws_is_power_of_two_harness(void) {
    /* nondeterministic input */
    size_t x = nondet_uint64_t();
    __CPROVER_assume(x <= SIZE_MAX);

    /* frame condition: snapshot some unrelated memory */
    size_t snapshot[10];
    size_t before[10];
    for (size_t i = 0; i < 10; ++i) {
        snapshot[i] = nondet_uint64_t();
        before[i]   = snapshot[i];
    }

    /* call the function under test */
    bool result = aws_is_power_of_two(x);

    /* postcondition: result matches the mathematical definition */
    __CPROVER_assert(
        result == (x != 0 && (x & (x - 1)) == 0),
        "aws_is_power_of_two returns correct boolean value"
    );

    /* frame condition: function must not modify unrelated memory */
    for (size_t i = 0; i < 10; ++i) {
        __CPROVER_assert(
            snapshot[i] == before[i],
            "aws_is_power_of_two does not modify external memory"
        );
    }

    return 0;
}
