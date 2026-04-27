#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

#define MAX_STRING_LEN 32
#define MAX_BUF_LEN 32

void aws_string_eq_byte_buf_harness(void) {
    /* Nondeterministically decide if str and buf are NULL */
    bool str_is_null;
    bool buf_is_null;

    struct aws_string *str = NULL;
    struct aws_byte_buf buf_storage;
    struct aws_byte_buf *buf = NULL;

    if (!str_is_null) {
        /* Create a valid aws_string */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);
        str = (struct aws_string *)malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);
        *(struct aws_allocator **)&str->allocator = NULL;
        *(size_t *)&str->len = str_len;
        ((uint8_t *)str->bytes)[str_len] = '\0';
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!buf_is_null) {
        /* Create a valid aws_byte_buf */
        buf = &buf_storage;
        size_t buf_len;
        size_t buf_capacity;
        __CPROVER_assume(buf_len <= MAX_BUF_LEN);
        __CPROVER_assume(buf_capacity >= buf_len);
        buf->len = buf_len;
        buf->capacity = buf_capacity;
        buf->allocator = NULL;
        if (buf->capacity > 0) {
            buf->buffer = (uint8_t *)malloc(buf->capacity);
            __CPROVER_assume(buf->buffer != NULL);
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
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }

    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->buffer == old_buf_buffer);
        assert(buf->allocator == old_buf_allocator);
        assert(aws_byte_buf_is_valid(buf));
    }

    /* === Correctness assertions === */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    if (str == NULL && buf != NULL) {
        assert(result == false);
    }

    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    if (str != NULL && buf != NULL && str->len != buf->len) {
        assert(result == false);
    }
}
