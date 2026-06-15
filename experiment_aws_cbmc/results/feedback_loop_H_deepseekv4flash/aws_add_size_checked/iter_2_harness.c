#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r = malloc(sizeof(*r));
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
        assert(a + b >= a);
        assert(a + b >= b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(*r == a + b);
        assert(a + b < a || a + b < b);
    }
}
