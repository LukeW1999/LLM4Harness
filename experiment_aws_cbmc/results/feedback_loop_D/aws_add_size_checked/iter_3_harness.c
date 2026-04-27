#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#define AWS_OP_SUCCESS 0
#define AWS_OP_ERR -1

#define SIZE_BITS 64  // Assuming 64-bit for this example

int aws_add_u64_checked(uint64_t a, uint64_t b, uint64_t *r);
int aws_add_u32_checked(uint32_t a, uint32_t b, uint32_t *r);
int aws_add_size_checked(size_t a, size_t b, size_t *r);

int aws_raise_error(int error_code) {
    return error_code;
}

void aws_add_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r_old;
    size_t r_new;

    // Save old state
    r_old = nondet_size_t();
    r_new = r_old;

    // Add assumptions to prevent overflow
    __CPROVER_assume(a <= SIZE_MAX - b);

    int result = aws_add_size_checked(a, b, &r_new);

    if (result == AWS_OP_SUCCESS) {
        assert(r_new == a + b);
    } else if (result == AWS_OP_ERR) {
        assert(r_new == r_old);
    }
}
