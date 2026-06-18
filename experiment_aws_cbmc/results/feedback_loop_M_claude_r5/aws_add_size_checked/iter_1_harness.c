#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_add_size_checked_harness() {
    /* 1. Declare inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    /* 2. Call function under test */
    int result = aws_add_size_checked(a, b, &r);

    /* 3. Assert postconditions for both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r must equal a + b, and no overflow occurred */
        assert(r == a + b);
        /* Verify no overflow: if b > 0, then a <= SIZE_MAX - b */
        if (b > 0) {
            assert(a <= (SIZE_MAX - b));
        }
    } else {
        /* On failure: overflow was detected */
        assert(result == AWS_OP_ERR);
        /* Overflow condition: b > 0 and a > SIZE_MAX - b */
        assert(b > 0 && a > (SIZE_MAX - b));
        /* r is not written on failure — no assertion needed for r */
    }

    /* 4. Verify the overflow detection logic is complete and correct:
          success iff no overflow */
    bool would_overflow = (b > 0) && (a > (SIZE_MAX - b));
    if (would_overflow) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a + b);
    }
}
