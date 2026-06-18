#include <aws/common/math.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_add_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t old_r = r;
    
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(uint64_t)));
    
    int result = aws_add_u64_checked(a, b, &r);
    
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }
}

void aws_add_u32_checked_harness() {
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    uint32_t old_r = r;
    
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(uint32_t)));
    
    int result = aws_add_u32_checked(a, b, &r);
    
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = r;
    
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&r, sizeof(size_t)));
    
    int result = aws_add_size_checked(a, b, &r);
    
    if (result == AWS_OP_SUCCESS) {
        assert(r == a + b);
    } else {
        assert(r == old_r);
    }
}
