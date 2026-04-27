// === STEP 1: SUCCESS PATH ===
// When aws_mul_size_saturating returns a value (successful multiplication without overflow):
//   - The return value is the product of a and b
//
// === STEP 2: FAILURE PATH ===
// When aws_mul_size_saturating returns SIZE_MAX (overflow):
//   - The return value is SIZE_MAX
//
// === STEP 3: FRAME CONDITIONS ===
// For the parameters of aws_mul_size_saturating, there are no structs, only primitive types:
//   a (size_t):
//     - UNCHANGED always
//   b (size_t):
//     - UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - No structs to validate
```

```c
#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t old_a = a;
    size_t old_b = b;

    size_t result = aws_mul_size_saturating(a, b);

    // Step 1: Success path
    if (result != SIZE_MAX) {
        assert(result == a * b);
    }

    // Step 2: Failure path
    if (result == SIZE_MAX) {
        assert(a * b > SIZE_MAX);
    }

    // Step 3: Frame conditions
    assert(a == old_a);
    assert(b == old_b);
}
