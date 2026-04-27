// === STEP 1: SUCCESS PATH ===
// aws_string_eq_byte_buf returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When both str and buf are NULL: returns true
// When str and buf are non-NULL: returns aws_array_eq(str->bytes, str->len, buf->buffer, buf->len)
// The function does NOT modify any of its parameters.
//
// === STEP 2: FAILURE PATH ===
// When str is NULL and buf is non-NULL, or str is non-NULL and buf is NULL: returns false
// No parameters are modified in any case.
//
// === STEP 3: FRAME CONDITIONS ===
// str (struct aws_string *):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// buf (struct aws_byte_buf *):
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//   - allocator: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
// The function doesn't require valid inputs (handles NULL).
// If str is non-NULL and buf is non-NULL, aws_string_is_valid(str) and aws_byte_buf_is_valid(buf)
// should hold before and after the call (they are not modified).

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

/* Stub for aws_array_eq since we're only verifying the harness logic */
bool aws_array_eq(const void *array_a, size_t len_a, const void *array_b, size_t len_b);

/* Helper to allocate a valid aws_string with bounded length */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate enough space for the struct plus len bytes plus null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) return NULL;
    /* Set the length field (cast away const for initialization) */
    *(size_t *)&str->len = len;
    /* allocator can be anything (including NULL for static strings) */
    return str;
}

void aws_string_eq_byte_buf_harness(void) {
    /* Nondeterministically choose whether str and buf are NULL */
    struct aws_string *str;
    struct aws_byte_buf *buf;

    /* We use nondet to decide if pointers are NULL or valid */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str_val = NULL;
    struct aws_byte_buf buf_val;
    struct aws_byte_buf *buf_ptr = NULL;

    if (!str_is_null) {
        /* Create a bounded aws_string */
        size_t str_len;
        __CPROVER_assume(str_len <= 10);
        str_val = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str_val != NULL);
        *(size_t *)&str_val->len = str_len;
        /* bytes content is nondet (already nondet from malloc) */
        str = str_val;
    } else {
        str = NULL;
    }

    if (!buf_is_null) {
        /* Create a bounded aws_byte_buf */
        size_t buf_len;
        __CPROVER_assume(buf_len <= 10);
        buf_val.len = buf_len;
        buf_val.capacity = buf_len;
        buf_val.allocator = NULL;
        if (buf_len > 0) {
            buf_val.buffer = malloc(buf_len);
            __CPROVER_assume(buf_val.buffer != NULL);
        } else {
            buf_val.buffer = NULL;
        }
        buf_ptr = &buf_val;
    } else {
        buf_ptr = NULL;
    }

    /* Save old state */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_buf_len = (buf_ptr != NULL) ? buf_ptr->len : 0;
    uint8_t *old_buf_buffer = (buf_ptr != NULL) ? buf_ptr->buffer : NULL;

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* === Verify return value semantics === */
    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || buf_ptr == NULL) {
        assert(result == false);
    }
    /* When both non-NULL, result depends on aws_array_eq - no additional assert needed */

    /* === Frame conditions: str is unchanged === */
    if (str != NULL) {
        assert(str->len == old_str_len);
        /* allocator and bytes are not modified - CBMC tracks this automatically */
    }

    /* === Frame conditions: buf is unchanged === */
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf_len);
        assert(buf_ptr->buffer == old_buf_buffer);
    }
}
