#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_round_up_to_power_of_two_harness() {
    size_t n;
    size_t result;
    size_t *r = &result;

    // Precondition: r must be a valid pointer
    __CPROVER_assume(r != NULL);

    // Call the function
    int ret = aws_round_up_to_power_of_two(n, r);

    // Postconditions
    if (ret == AWS_OP_SUCCESS) {
        // On success, *r is the smallest power of two >= n
        assert(*r >= n);
        assert((*r & (*r - 1)) == 0); // *r is a power of two
        // No smaller power of two >= n exists
        if (n > 0) {
            assert(*r / 2 < n);
        }
    } else {
        // On failure, *r is unchanged (but we don't know its initial value)
        // The function returns AWS_OP_ERR if overflow would occur
        // No assertion about *r needed on failure
    }

    // Unchanged: n is not modified
    // (n is passed by value, so it's unchanged by definition)
}
