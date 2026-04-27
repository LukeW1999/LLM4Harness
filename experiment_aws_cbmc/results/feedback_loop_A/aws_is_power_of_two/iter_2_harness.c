#include <aws/common/common.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_is_power_of_two_harness() {
    size_t n = nondet_size_t();

    /* 1. Declare and bound data structures */
    /* No specific data structures to declare or bound for this function */

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    /* No specific state to save for this function */

    /* 3. Call function under test */
    bool result = aws_is_power_of_two(n);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(n > 0 && (n & (n - 1)) == 0);
    } else {
        assert(n == 0 || (n & (n - 1)) != 0);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No other fields to assert for this function */

    /* 6. Assert validity invariants always holds */
    /* No specific validity invariants to assert for this function */
}
