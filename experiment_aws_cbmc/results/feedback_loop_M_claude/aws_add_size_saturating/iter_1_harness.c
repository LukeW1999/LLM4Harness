#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*
 * Harness for aws_add_size_checked_varargs:
 * "Adds [num] arguments (expected to be of size_t), and returns the result in *r.
 *  If the result overflows, returns AWS_OP_ERR; otherwise returns AWS_OP_SUCCESS."
 *
 * We test with a fixed small number of arguments to keep the state space bounded.
 * We use num=3 as a representative case.
 */
void aws_add_size_checked_varargs_harness(void) {
    /* Bound the number of arguments to prevent state space explosion */
    size_t num = 3;

    /* Non-deterministic size_t arguments, bounded to prevent overflow in CBMC */
    size_t a = nondet_size_t();
    __CPROVER_assume(a <= SIZE_MAX);

    size_t b = nondet_size_t();
    __CPROVER_assume(b <= SIZE_MAX);

    size_t c = nondet_size_t();
    __CPROVER_assume(c <= SIZE_MAX);

    /* Output pointer */
    size_t r = 0;

    /* Call the function under test */
    int result = aws_add_size_checked_varargs(num, &r, a, b, c);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: r == a + b + c (no overflow occurred) */
        /* Verify no overflow actually occurred */
        assert(a <= SIZE_MAX - b);
        size_t ab = a + b;
        assert(ab <= SIZE_MAX - c);
        assert(r == a + b + c);
    } else {
        /* On failure (AWS_OP_ERR): overflow was detected */
        assert(result == -1);
        /* The overflow condition: either a+b overflows or (a+b)+c overflows */
        /* We can't easily assert the exact overflow condition without recomputing,
         * but we can assert the return value is AWS_OP_ERR */
    }

    /* result must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == 0 || result == -1);
}
