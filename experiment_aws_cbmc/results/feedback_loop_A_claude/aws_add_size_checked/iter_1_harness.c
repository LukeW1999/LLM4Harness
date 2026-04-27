#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <aws/common/common.h>
#include <aws/common/math.h>

void aws_add_size_checked_harness() {
    /* 1. Declare non-deterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    /* 2. Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* 3. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r must equal a + b (no overflow occurred) */
        assert(r == a + b);
        /* The addition did not overflow */
        assert(a <= SIZE_MAX - b || b == 0);
    } else {
        /* On failure: result must be AWS_OP_ERR */
        assert(result == -1);
        /* Overflow must have occurred: b > 0 and a > SIZE_MAX - b */
        assert(b > 0 && a > SIZE_MAX - b);
    }

    /* 4. Verify the overflow detection logic is correct:
          If no overflow, success must be returned */
    if (b == 0 || a <= SIZE_MAX - b) {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a + b);
    } else {
        assert(result == -1);
    }
}
