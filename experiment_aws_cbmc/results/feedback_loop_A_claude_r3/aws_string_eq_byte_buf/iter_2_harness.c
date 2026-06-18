#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_string_eq_byte_buf_harness(void) {
    /* str can be NULL or a valid aws_string */
    struct aws_string *str = NULL;
    if (nondet_bool()) {
        size_t str_len;
        __CPROVER_assume(str_len <= 10);
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);
        size_t *len_ptr = (size_t *)&str->len;
        *len_ptr = str_len;
        struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
        *alloc_ptr = aws_default_allocator();
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* buf can be NULL or a valid aws_byte_buf */
    struct aws_byte_buf *buf = NULL;
    struct aws_byte_buf buf_val;
    if (nondet_bool()) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf_val, 10));
        ensure_byte_buf_has_allocated_buffer_member(&buf_val);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf_val));
        buf = &buf_val;
    }

    /* Save old state */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    size_t old_buf_len = (buf != NULL) ? buf->len : 0;
    size_t old_buf_capacity = (buf != NULL) ? buf->capacity : 0;
    struct aws_allocator *old_buf_allocator = (buf != NULL) ? buf->allocator : NULL;
    uint8_t *old_buf_buffer = (buf != NULL) ? buf->buffer : NULL;

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Assert postconditions */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    /* Assert immutability */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        assert(aws_string_is_valid(str));
    }

    if (buf != NULL) {
        assert(buf->len == old_buf_len);
        assert(buf->capacity == old_buf_capacity);
        assert(buf->allocator == old_buf_allocator);
        assert(buf->buffer == old_buf_buffer);
        assert(aws_byte_buf_is_valid(buf));
    }

    assert(result == true || result == false);
}
