// === STEP 1: SUCCESS PATH ===
// When aws_string_eq_c_str returns true:
//   - str->bytes: UNCHANGED
//   - str->len: UNCHANGED
//   - c_str: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_string_eq_c_str returns false:
//   - str->bytes: UNCHANGED
//   - str->len: UNCHANGED
//   - c_str: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// c_str (const char*):
//   - UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(&str): YES (must hold after call)
//   - c_str does not have a validity check function

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_eq_c_str_harness() {
    struct aws_string *str = malloc(sizeof(struct aws_string));
    __CPROVER_assume(aws_string_is_valid(str));
    const char *c_str = (const char *)malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);

    struct aws_string old_str = *str;
    const char *old_c_str = c_str;

    bool result = aws_string_eq_c_str(str, c_str);

    // Step 1: Success path assertions
    if (result) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
        assert(c_str == old_c_str);
    }

    // Step 2: Failure path assertions
    if (!result) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert(str->bytes == old_str.bytes);
        assert(c_str == old_c_str);
    }

    // Step 3: Frame conditions
    assert(str->allocator == old_str.allocator);
    assert(str->len == old_str.len);
    assert(str->bytes == old_str.bytes);
    assert(c_str == old_c_str);

    // Step 4: Validity invariants
    assert(aws_string_is_valid(str));
}
