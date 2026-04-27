// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_from_string returns a valid aws_byte_cursor:
//   - The returned cursor's ptr: CHANGES to aws_string_bytes(src)
//   - The returned cursor's len: CHANGES to src->len
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_from_string is called with src as NULL:
//   - The returned cursor's ptr: CHANGES to NULL
//   - The returned cursor's len: CHANGES to 0
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   src (struct aws_string*):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(src): YES (must hold before call)
```

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_string_harness() {
    struct aws_string *src;
    __CPROVER_assume(src != NULL);
    __CPROVER_assume(aws_string_is_valid(src));

    struct aws_string old_src = *src;

    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    // Step 1: Success Path
    if (src != NULL) {
        assert(result.ptr == aws_string_bytes(src));
        assert(result.len == src->len);
    }
    // Step 2: Failure Path
    else {
        assert(result.ptr == NULL);
        assert(result.len == 0);
    }

    // Step 3: Frame Conditions
    assert(src->allocator == old_src.allocator);
    assert(src->len == old_src.len);
    assert(src->bytes == old_src.bytes);

    // Step 4: Validity Invariants
    assert(aws_string_is_valid(src));
}
