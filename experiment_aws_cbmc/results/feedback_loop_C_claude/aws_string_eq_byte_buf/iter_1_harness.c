// === STEP 1: SUCCESS PATH ===
// aws_string_eq_byte_buf returns a bool (true/false), not AWS_OP_SUCCESS/ERR
// When it returns true:
//   - str and buf have equal bytes and equal lengths
//   - No fields are modified
//
// === STEP 2: FAILURE PATH ===
// When it returns false:
//   - str is NULL and buf is not NULL, or vice versa, or bytes differ
//   - No fields are modified
//
// === STEP 3: FRAME CONDITIONS ===
// str (const struct aws_string *):
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// buf (struct aws_byte_buf *):
//   - allocator: UNCHANGED always
//   - buffer: UNCHANGED always
//   - len: UNCHANGED always
//   - capacity: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(str): YES (if str != NULL, must hold before and after)
//   - aws_byte_buf_is_valid(buf): YES (if buf != NULL, must hold before and after)

#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <assert.h>

/* Needed for aws_array_eq used internally */
extern bool aws_array_eq(const void *array_a, size_t len_a, const void *array_b, size_t len_b);

#define MAX_STRING_LEN 32
#define MAX_BUF_LEN 32

struct aws_string *make_arbitrary_aws_string_nondet(struct aws_allocator *allocator, size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate enough memory for the header + len + 1 null terminator */
    struct aws_string *str = bounded_malloc(sizeof(struct aws_string) + len + 1);
    /* Initialize fields - cast away const for initialization */
    *(struct aws_allocator **)&str->allocator = allocator;
    *(size_t *)&str->len = len;
    /* bytes are nondet by default from bounded_malloc */
    /* Ensure null terminator */
    ((uint8_t *)str->bytes)[len] = '\0';
    return str;
}

void aws_string_eq_byte_buf_harness(void) {
    /* Nondeterministically decide if str and buf are NULL */
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf buf_storage;
    struct aws_byte_buf *buf = NULL;

    if (!str_is_null) {
        /* Create a valid aws_string */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);
        str = bounded_malloc(sizeof(struct aws_string) + str_len + 1);
        *(struct aws_allocator **)&str->allocator = NULL; /* static-like */
        *(size_t *)&str->len = str_len;
        ((uint8_t *)str->bytes)[str_len] = '\0';
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!buf_is_null) {
        /* Create a valid aws_byte_buf */
        buf = &buf_storage;
        buf->len = nondet_size_t();
        buf->capacity = nondet_size_t();
        __CPROVER_assume(buf->len <= MAX_BUF_LEN);
        __CPROVER_assume(buf->capacity >= buf->len);
        buf->allocator = NULL;
        if (buf->capacity > 0) {
            buf->buffer = bounded_malloc(buf->capacity);
        } else {
            buf->buffer = NULL;
        }
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* Save old state */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    size_t old_buf_len = (buf != NULL) ? buf->len : 0;
    size_t old_buf_capacity = (buf != NULL) ? buf->capacity : 0;
    uint8_t *old_buf_buffer = (buf != NULL) ? buf->buffer : NULL;
    struct aws_allocator *old_buf_allocator = (buf != NULL) ? buf->allocator : NULL;

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* === Frame condition assertions === */
    /* str fields unchanged */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }

    /* buf fields unchanged */
    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->buffer == old_buf_buffer);
        assert(buf->allocator == old_buf_allocator);
        assert(aws_byte_buf_is_valid(buf));
    }

    /* === Correctness assertions === */
    /* If both NULL, result must be true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    /* If exactly one is NULL, result must be false */
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    /* If both non-NULL and lengths differ, result must be false */
    if (str != NULL && buf != NULL && str->len != buf->len) {
        assert(result == false);
    }
}
