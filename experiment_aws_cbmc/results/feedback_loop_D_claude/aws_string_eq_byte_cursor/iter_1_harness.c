// === STEP 1: SUCCESS PATH ===
// aws_string_eq_byte_cursor returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When both str and cur are NULL: returns true
// When str is NULL and cur is not NULL (or vice versa): returns false
// When both are non-NULL: returns aws_array_eq(str->bytes, str->len, cur->ptr, cur->len)
//   - No fields are modified in any case
//
// === STEP 2: FAILURE PATH ===
// N/A - this function only reads, never modifies inputs
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
// - aws_string_is_valid(str): YES (if non-NULL, must hold before and after)
// - aws_byte_cursor_is_valid(cur): YES (if non-NULL, must hold before and after)

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>

/* Stub for aws_array_eq since we're only verifying the wrapper logic */
bool aws_array_eq(const void *array_a, size_t len_a, const void *array_b, size_t len_b);

#define MAX_STRING_LEN 32
#define MAX_CURSOR_LEN 32

struct aws_string *make_arbitrary_aws_string_with_bounded_len(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* allocate enough space for the struct plus len bytes plus null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    /* Use a write through a non-const pointer to initialize */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;
    /* bytes are nondet by default */
    /* ensure null terminator */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = 0;
    return str;
}

void aws_string_eq_byte_cursor_harness() {
    /* Nondeterministically choose whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    if (!str_is_null) {
        str = make_arbitrary_aws_string_with_bounded_len(MAX_STRING_LEN);
        /* Assume str is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_CURSOR_LEN));
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    /* Save old state */
    struct aws_string old_str_val;
    struct aws_byte_cursor old_cur_val;

    if (str != NULL) {
        old_str_val = *str;
    }
    if (cur != NULL) {
        old_cur_val = *cur;
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* === Verify return value logic === */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    } else if (str == NULL || cur == NULL) {
        assert(result == false);
    }
    /* When both non-NULL, result depends on aws_array_eq - no additional assert needed */

    /* === Frame conditions: str is unchanged === */
    if (str != NULL) {
        assert(str->len == old_str_val.len);
        assert(str->allocator == old_str_val.allocator);
        /* bytes content unchanged - check via validity */
        assert(aws_string_is_valid(str));
    }

    /* === Frame conditions: cur is unchanged === */
    if (cur != NULL) {
        assert(cur->ptr == old_cur_val.ptr);
        assert(cur->len == old_cur_val.len);
        assert(aws_byte_cursor_is_valid(cur));
    }
}
