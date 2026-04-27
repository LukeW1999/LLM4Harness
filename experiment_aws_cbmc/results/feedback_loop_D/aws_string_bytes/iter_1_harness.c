// === STEP 1: SUCCESS PATH ===
// When aws_string_bytes returns a non-null value (or the successful value):
//   - str->bytes: RETURNS str->bytes (unchanged)

// === STEP 2: FAILURE PATH ===
// When aws_string_bytes returns NULL (or fails):
//   - str->allocator: UNCHANGED
//   - str->len: UNCHANGED
//   - str->bytes: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   str (struct aws_string):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: RETURNS str->bytes (unchanged)

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&str): YES (must hold after call)
```

```c
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_string_bytes_harness() {
    struct aws_string str;
    str.allocator = (struct aws_allocator *)nondet_ptr();
    str.len = nondet_size_t();
    str.bytes = (const uint8_t *)nondet_ptr();

    // Save old state
    struct aws_string old_str = str;

    const uint8_t *result = aws_string_bytes(&str);

    // Assert frame conditions
    assert(str.allocator == old_str.allocator);
    assert(str.len == old_str.len);
    assert(result == old_str.bytes);

    // Assert validity invariants
    assert(aws_string_is_valid(&str));
}
