#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdbool.h>

// Mock implementation of the function to be verified
int aws_add_size_checked(size_t a, size_t b, size_t *r) {
    size_t result = a + b;
    if (result < a || result < b) {
        return -1; // AWS_OP_ERR
    }
    *r = result;
    return 0; // AWS_OP_SUCCESS
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    // Precondition: r_ptr must not be NULL
    __CPROVER_assume(r_ptr != NULL);

    int result = aws_add_size_checked(a, b, r_ptr);

    if (result == 0) { // AWS_OP_SUCCESS
        // Postcondition: a + b should not overflow and r should be equal to a + b
        assert(a + b >= a);
        assert(a + b >= b);
        assert(r == a + b);
    } else if (result == -1) { // AWS_OP_ERR
        // Postcondition: a + b should overflow
        assert(a + b < a || a + b < b);
    } else {
        // Unreachable: function should only return AWS_OP_SUCCESS or AWS_OP_ERR
        assert(false);
    }
}
