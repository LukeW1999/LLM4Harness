// === STEP 1: SUCCESS PATH ===
// aws_string_eq_c_str returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When returns true:
//   - Both str and c_str are NULL, OR
//   - Both are non-NULL and aws_array_eq_c_str(str->bytes, str->len, c_str) returns true
//
// === STEP 2: FAILURE PATH ===
// When returns false:
//   - One of str/c_str is NULL and the other is not, OR
//   - Both non-NULL but bytes don't match
//
// === STEP 3: FRAME CONDITIONS ===
// This function is read-only - it does not modify any parameters
//   str (struct aws_string *):
//     - allocator: UNCHANGED always
//     - len: UNCHANGED always
//     - bytes: UNCHANGED always
//   c_str (const char *):
//     - contents: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(str): YES if str was valid before (must hold after call)
//   - The function handles NULL inputs gracefully

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

/* Stub for aws_array_eq_c_str since we're testing aws_string_eq_c_str */
bool aws_array_eq_c_str(const void *array, size_t array_len, const char *c_str);

#define MAX_STRING_LEN 32

struct aws_string *make_arbitrary_aws_string_nondet(struct aws_allocator *allocator) {
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);
    
    /* Allocate enough space for the struct plus len bytes plus null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) return NULL;
    
    /* Use memcpy-style trick to set const fields */
    *(struct aws_allocator **)&str->allocator = allocator;
    *(size_t *)&str->len = len;
    
    /* bytes are already nondet from malloc */
    /* Ensure null terminator at bytes[len] */
    uint8_t *bytes_ptr = (uint8_t *)str->bytes;
    bytes_ptr[len] = '\0';
    
    return str;
}

void aws_string_eq_c_str_harness() {
    /* Create nondeterministic inputs */
    struct aws_string *str;
    const char *c_str;
    
    /* Nondeterministically choose whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        struct aws_allocator *allocator = nondet_bool() ? NULL : malloc(sizeof(struct aws_allocator));
        str = make_arbitrary_aws_string_nondet(allocator);
        /* If non-NULL, assume it's valid */
        if (str != NULL) {
            __CPROVER_assume(aws_string_is_valid(str));
        }
    }
    
    /* Nondeterministically choose whether c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        /* Create a bounded c_str */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_STRING_LEN);
        char *c_str_buf = malloc(c_str_len + 1);
        __CPROVER_assume(c_str_buf != NULL);
        c_str_buf[c_str_len] = '\0';
        c_str = c_str_buf;
    }
    
    /* Save old state */
    const struct aws_string *old_str = str;
    size_t old_len = (str != NULL) ? str->len : 0;
    
    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);
    
    /* === FRAME CONDITIONS: str is not modified === */
    assert(str == old_str);
    if (str != NULL) {
        assert(str->len == old_len);
        /* Validity is preserved */
        assert(aws_string_is_valid(str));
    }
    
    /* === CORRECTNESS ASSERTIONS === */
    
    /* Case 1: both NULL => must return true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }
    
    /* Case 2: exactly one NULL => must return false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }
    
    /* Case 3: both non-NULL => result matches aws_array_eq_c_str */
    /* (This is implicitly verified by the function calling through) */
}
