// === STEP 1: SUCCESS PATH ===
// aws_mul_size_saturating(a, b) returns a * b when no overflow occurs.
// The function takes two size_t values and returns their product saturated at SIZE_MAX.
// No output pointer - the result is the return value directly.
// When a * b does NOT overflow: returns a * b
// When a * b DOES overflow: returns SIZE_MAX
//
// === STEP 2: FAILURE PATH ===
// This function never "fails" in the AWS_OP_ERR sense - it always returns a value.
// The saturating behavior means:
//   - If overflow would occur: returns SIZE_MAX
//   - Otherwise: returns a * b
//
// === STEP 3: FRAME CONDITIONS ===
// This function has no struct parameters - only two size_t inputs and a size_t return value.
// No memory is modified. No pointers involved.
// The inputs a and b are passed by value and cannot be modified.
//
// === STEP 4: VALIDITY INVARIANTS ===
// The return value r must satisfy:
//   - If a == 0 || b == 0: r == 0
//   - If overflow (a * b > SIZE_MAX): r == SIZE_MAX
//   - Otherwise: r == a * b
// More precisely, using 64-bit arithmetic to check:
//   - If SIZE_MAX == UINT64_MAX: need __uint128_t or similar
//   - If SIZE_MAX == UINT32_MAX: can use uint64_t to check
// The key property: r <= SIZE_MAX always (trivially true for size_t)
// And: if no overflow, r == a * b
// And: if overflow, r == SIZE_MAX

#include <aws/common/math.h>
#include <stdint.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = aws_mul_size_saturating(a, b);

    // The result must always be a valid size_t (trivially true)
    // Check correctness based on overflow detection

#if SIZE_MAX == UINT32_MAX
    // On 32-bit platforms, use uint64_t to detect overflow
    uint64_t wide = (uint64_t)a * (uint64_t)b;
    if (wide > (uint64_t)SIZE_MAX) {
        // Overflow: result must be SIZE_MAX
        assert(result == SIZE_MAX);
    } else {
        // No overflow: result must equal a * b
        assert(result == (size_t)wide);
    }
#elif SIZE_MAX == UINT64_MAX
    // On 64-bit platforms, use __uint128_t to detect overflow
    __uint128_t wide = (__uint128_t)a * (__uint128_t)b;
    if (wide > (__uint128_t)SIZE_MAX) {
        // Overflow: result must be SIZE_MAX
        assert(result == SIZE_MAX);
    } else {
        // No overflow: result must equal a * b
        assert(result == (size_t)wide);
    }
#endif

    // Additional sanity checks:
    // If either operand is 0, result must be 0
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    // If both operands are 1, result must be 1
    if (a == 1 && b == 1) {
        assert(result == 1);
    }

    // Result is always <= SIZE_MAX (trivially true for size_t, but let's be explicit)
    assert(result <= SIZE_MAX);

    // If a == 1, result must equal b (no overflow possible unless b itself is SIZE_MAX which is fine)
    if (a == 1) {
        assert(result == b);
    }

    // If b == 1, result must equal a
    if (b == 1) {
        assert(result == a);
    }
}
