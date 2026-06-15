#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_add_size_checked_harness(void) {
    size_t a = (size_t)nondet_uint64_t();
    __CPROVER_assume(a <= MAX_BUFFER_SIZE);

    size_t b = (size_t)nondet_uint64_t();
    __CPROVER_assume(b <= MAX_BUFFER_SIZE);

    size_t r = (size_t)nondet_uint64_t();
    __CPROVER_assume(r <= MAX_BUFFER_SIZE);

    size_t old_a = a;
    size_t old_b = b;
    size_t old_r = r;

    int result = aws_add_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }

    assert(a == old_a);
    assert(b == old_b);
}
