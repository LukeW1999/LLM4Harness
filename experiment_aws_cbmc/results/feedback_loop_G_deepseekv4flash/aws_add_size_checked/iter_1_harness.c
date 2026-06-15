#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/math.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r = malloc(sizeof(size_t));
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    size_t old_r = *r;

    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        // On success, the sum must not overflow and *r must equal a + b
        __CPROVER_assert(*r == a + b, "On success, *r equals a + b");
        __CPROVER_assert(!((b > 0) && (a > SIZE_MAX - b)), "No overflow on success");
    } else {
        // On failure, *r must be unchanged and overflow must have occurred
        __CPROVER_assert(*r == old_r, "On failure, *r unchanged");
        __CPROVER_assert((b > 0) && (a > SIZE_MAX - b), "Overflow condition holds on failure");
    }

    // Return value must be either success or error
    __CPROVER_assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR, "Return value is valid");

    // The function does not modify a or b (passed by value)
    // No other side effects to check
}
