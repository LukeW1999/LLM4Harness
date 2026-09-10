#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness() {
    bool str_is_null = nondet_bool();
    bool buf_is_null = nondet_bool();

    struct aws_string *str = NULL;
    struct aws_byte_buf *buf = NULL;

    struct aws_string *str_storage = NULL;
    if (!str_is_null) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        str_storage = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str_storage != NULL);

        *(struct aws_allocator **)&str_storage->allocator = aws_default_allocator();
        *(size_t *)&str_storage->len = str_len;

        __CPROVER_assume(aws_string_is_valid(str_storage));
        str = str_storage;
    }

    struct aws_byte_buf buf_storage;
    if (!buf_is_null) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf_storage, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buf_storage);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf_storage));
        buf = &buf_storage;
    }

    struct aws_string *old_str = str;
    struct aws_byte_buf old_buf_val;
    if (buf != NULL) {
        old_buf_val = *buf;
    }

    bool result = aws_string_eq_byte_buf(str, buf);

    /* Expert's postconditions */
    if (result && str) {
        assert(str->len == buf_storage.len);
        assert_bytes_match(str->bytes, buf_storage.buffer, str->len);
        assert(aws_string_is_valid(str));
    }

    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }

    /* Additional postconditions from my harness */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    if (str != NULL && buf != NULL) {
        assert(str == old_str);
        assert(str->len == old_str->len);
        assert(str->allocator == old_str->allocator);

        assert(buf->len == old_buf_val.len);
        assert(buf->capacity == old_buf_val.capacity);
        assert(buf->buffer == old_buf_val.buffer);
        assert(buf->allocator == old_buf_val.allocator);

        assert(aws_string_is_valid(str));
        assert(aws_byte_buf_is_valid(buf));
    }

    assert(str == old_str);

    if (buf != NULL) {
        assert(buf->len == old_buf_val.len);
        assert(buf->capacity == old_buf_val.capacity);
        assert(buf->buffer == old_buf_val.buffer);
        assert(buf->allocator == old_buf_val.allocator);
        assert(aws_byte_buf_is_valid(buf));
    }

    if (str != NULL) {
        assert(str->len == old_str->len);
        assert(str->allocator == old_str->allocator);
        assert(aws_string_is_valid(str));
    }
}
