#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "proof_helpers/make_common_data_structures.h"

int aws_add_u64_checked(uint64_t a, uint64_t b, uint64_t *r);
int aws_add_u32_checked(uint32_t a, uint32_t b, uint32_t *r);
int aws_add_size_checked(size_t a, size_t b, size_t *r);

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t *r_ptr = &r;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(uint64_t)));

    int result = aws_add_u64_checked(a, b, r_ptr);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(!(b > 0 && a > (UINT64_MAX - b)));
    } else if (result == AWS_OP_ERR) {
        assert(b > 0 && a > (UINT64_MAX - b));
    }
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    uint32_t *r_ptr = &r;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(uint32_t)));

    int result = aws_add_u32_checked(a, b, r_ptr);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(!(b > 0 && a > (UINT32_MAX - b)));
    } else if (result == AWS_OP_ERR) {
        assert(b > 0 && a > (UINT32_MAX - b));
    }
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t *r_ptr = &r;

    __CPROVER_assume(AWS_MEM_IS_WRITABLE(r_ptr, sizeof(size_t)));

    int result = aws_add_size_checked(a, b, r_ptr);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
        assert(!(b > 0 && a > (SIZE_MAX - b)));
    } else if (result == AWS_OP_ERR) {
        assert(b > 0 && a > (SIZE_MAX - b));
    }
}
