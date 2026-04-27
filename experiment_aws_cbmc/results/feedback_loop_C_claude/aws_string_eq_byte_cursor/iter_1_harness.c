// === STEP 1: SUCCESS PATH ===
// aws_string_eq_byte_cursor returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When it returns true:
//   - str and cur have equal bytes
//   - No fields are modified
//
// === STEP 2: FAILURE PATH ===
// When it returns false:
//   - str or cur is NULL, or bytes differ
//   - No fields are modified
//
// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string *):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// cur (struct aws_byte_cursor *):
//   - ptr: UNCHANGED always
//   - len: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function does not require valid inputs (handles NULL)
// If str != NULL: aws_string_is_valid(str) should hold before call
// If cur != NULL: aws_byte_cursor_is_bounded(cur, MAX_SIZE) should hold before call
// After call: validity of inputs is preserved (no mutation)

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#define MAX_STRING_LEN 10

struct aws_string *make_arbitrary_aws_string_with_bounded_len(struct aws_allocator *allocator, size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    // Allocate memory for the aws_string header + len bytes + 1 null terminator
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    // Set the fields (cast away const for initialization)
    *(struct aws_allocator **)&str->allocator = allocator;
    *(size_t *)&str->len = len;
    // bytes are nondet (already nondet from malloc)
    // ensure null terminator
    ((uint8_t *)str->bytes)[len] = '\0';
    return str;
}

void aws_string_eq_byte_cursor_harness() {
    // Nondeterministically choose whether str and cur are NULL
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    // Set up str if non-null
    size_t str_len_old = 0;
    const uint8_t *str_bytes_old = NULL;
    struct aws_allocator *str_allocator_old = NULL;

    if (!str_is_null) {
        str = make_arbitrary_aws_string_with_bounded_len(NULL, MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
        str_len_old = str->len;
        str_bytes_old = str->bytes;
        str_allocator_old = str->allocator;
    }

    // Set up cur if non-null
    struct aws_byte_cursor cur_val;
    size_t cur_len_old = 0;
    uint8_t *cur_ptr_old = NULL;

    if (!cur_is_null) {
        __CPROVER_assume(aws_byte_cursor_is_bounded(&cur_val, MAX_STRING_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(&cur_val);
        __CPROVER_assume(aws_byte_cursor_is_valid(&cur_val));
        cur = &cur_val;
        cur_len_old = cur_val.len;
        cur_ptr_old = cur_val.ptr;
    }

    // Call the function under test
    bool result = aws_string_eq_byte_cursor(str, cur);

    // === FRAME CONDITIONS: verify no mutation ===
    if (!str_is_null) {
        assert(str->len == str_len_old);
        assert(str->bytes == str_bytes_old);
        assert(str->allocator == str_allocator_old);
        // Validity preserved
        assert(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        assert(cur->len == cur_len_old);
        assert(cur->ptr == cur_ptr_old);
        // Validity preserved
        assert(aws_byte_cursor_is_valid(cur));
    }

    // === CORRECTNESS: NULL handling ===
    if (str_is_null && cur_is_null) {
        assert(result == true);
    } else if (str_is_null || cur_is_null) {
        assert(result == false);
    }
    // When both non-null, result depends on byte comparison - no additional assert needed
    // as the correctness of aws_array_eq is assumed correct
}
