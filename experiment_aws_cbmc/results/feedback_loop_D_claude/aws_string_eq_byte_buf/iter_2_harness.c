#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <assert.h>

void aws_string_eq_byte_buf_harness(void) {
    struct aws_string *str;
    struct aws_byte_buf *buf_ptr;

    bool str_is_null;
    bool buf_is_null;

    struct aws_byte_buf buf_val;

    if (!str_is_null) {
        size_t str_len;
        __CPROVER_assume(str_len <= 10);
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);
        *(size_t *)&str->len = str_len;
        /* Ensure aws_string_is_valid(str): bytes must point to the inline data */
        /* In aws_string, bytes is a flexible array member at the end of the struct */
        /* aws_string_is_valid checks that str->bytes == (uint8_t *)(str + 1) */
        /* and str->bytes[str->len] == '\0' */
        /* The bytes field is const uint8_t[] at end of struct, so bytes == (uint8_t*)(str+1) automatically */
        /* We need to set the null terminator */
        uint8_t *bytes_ptr = (uint8_t *)(str + 1);
        bytes_ptr[str_len] = '\0';
        /* allocator can be anything */
    } else {
        str = NULL;
    }

    if (!buf_is_null) {
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

    /* Ensure preconditions hold */
    __CPROVER_assume(!str || aws_string_is_valid(str));
    __CPROVER_assume(!buf_ptr || aws_byte_buf_is_valid(buf_ptr));

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

    /* === Frame conditions: str is unchanged === */
    if (str != NULL) {
        assert(str->len == old_str_len);
    }

    /* === Frame conditions: buf is unchanged === */
    if (buf_ptr != NULL) {
        assert(buf_ptr->len == old_buf_len);
        assert(buf_ptr->buffer == old_buf_buffer);
    }
}
