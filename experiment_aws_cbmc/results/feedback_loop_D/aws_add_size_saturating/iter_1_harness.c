// === STEP 1: SUCCESS PATH ===
// When aws_add_size_saturating returns a value (successful addition without overflow):
//   - The return value is the sum of a and b if no overflow occurs

// === STEP 2: FAILURE PATH ===
// When aws_add_size_saturating returns SIZE_MAX (overflow):
//   - The return value is SIZE_MAX

// === STEP 3: FRAME CONDITIONS ===
// For the parameters of aws_add_size_saturating, there are no structs, only primitive types:
//   a (size_t): UNCHANGED always
//   b (size_t): UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - There are no structs to validate with aws_XXX_is_valid

#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>

void aws_add_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t old_a = a;
    size_t old_b = b;

    size_t result = aws_add_size_saturating(a, b);

    // Step 1: Success path assertions
    if (result != SIZE_MAX) {
        assert(result == old_a + old_b);
    }

    // Step 2: Failure path assertions
    if (result == SIZE_MAX) {
        assert(old_a + old_b < old_a || old_a + old_b < old_b); // Overflow condition
    }

    // Step 3: Frame conditions
    assert(a == old_a);
    assert(b == old_b);
}
