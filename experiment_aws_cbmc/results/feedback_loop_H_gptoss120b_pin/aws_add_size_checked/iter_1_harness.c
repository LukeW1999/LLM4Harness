#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* nondeterministic inputs */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* output pointer */
    size_t r_obj = nondet_size_t();   /* initial value may be anything */
    size_t *r = &r_obj;

    /* save old state of *r for the failure case */
    size_t old_r = r_obj;

    /* call the function under test */
    int result = aws_add_size_checked(a, b, r);

    /* result must be one of the defined return codes */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* on success the sum must be stored in *r */
        assert(*r == a + b);
        /* overflow must not have occurred */
        assert(!(b > 0 && a > (SIZE_MAX - b)));
    } else {
        /* on failure *r must remain unchanged */
        assert(*r == old_r);
    }

    /* the output pointer itself must not change */
    assert(r == &r_obj);
}
