#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministic allocation and bounding for str */
    struct aws_string *str = can_fail_malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    if (str) {
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_STRING_LEN);
        __CPROVER_assume(AWS_MEM_IS_READABLE(str->bytes, str->len));
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministic allocation and bounding for buf */
    struct aws_byte_buf *buf = can_fail_malloc(sizeof(struct aws_byte_buf));
    if (buf) {
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* Save old state for immutability checks */
    struct aws_string *old_str = NULL;
    struct store_byte_from_buffer old_buf_storage;
    size_t old_str_len;
    const uint8_t *old_str_bytes;
    if (str) {
        old_str = (struct aws_string *)malloc(sizeof(struct aws_string) + str->len);
        if (old_str) {
            *old_str = *str;
            // Note: only the first byte of the flexible array is copied; we manually copy len bytes
            for (size_t i = 0; i < str->len; i++) {
                old_str->bytes[i] = str->bytes[i];
            }
        }
        old_str_len = str->len;
        old_str_bytes = str->bytes;
    }
    if (buf) {
        save_byte_from_array(buf->buffer, buf->len, &old_buf_storage);
    }

    /* Call function under verification */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Assert postconditions */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    } else if (str == NULL || buf == NULL) {
        assert(result == false);
    }

    /* Unchanged fields */
    if (str) {
        assert(str->len == old_str_len);
        assert(str->bytes == old_str_bytes);
        /* bytes content unchanged */
        for (size_t i = 0; i < str->len; i++) {
            assert(str->bytes[i] == old_str->bytes[i]);
        }
        assert(aws_string_is_valid(str));
    }
    if (buf) {
        assert(buf->len == old_buf_storage.len);
        assert(buf->buffer == old_buf_storage.buffer);
        assert(buf->capacity == old_buf_storage.capacity);
        assert_byte_from_buffer_matches(buf->buffer, &old_buf_storage);
        assert(aws_byte_buf_is_valid(buf));
    }

    /* Cleanup to avoid memory leak (optional but good practice) */
    if (old_str) {
        free(old_str);
    }
}
