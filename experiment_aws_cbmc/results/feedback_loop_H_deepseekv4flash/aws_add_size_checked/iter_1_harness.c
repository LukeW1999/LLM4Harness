#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t *r = malloc(sizeof(*r));
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    int result = aws_add_u64_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(*r == a + b); // *r is still written on overflow
    }
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t *r = malloc(sizeof(*r));
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    int result = aws_add_u32_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(*r == a + b); // *r is still written on overflow
    }
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t *r = malloc(sizeof(*r));
    __CPROVER_assume(r != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r, sizeof(*r)));

    int result = aws_add_size_checked(a, b, r);

    if (result == AWS_OP_SUCCESS) {
        assert(*r == a + b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(*r == a + b); // *r is still written on overflow
    }
}
