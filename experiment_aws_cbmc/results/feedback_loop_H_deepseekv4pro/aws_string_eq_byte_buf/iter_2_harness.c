#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministically decide if str is NULL */
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;
    if (!str_is_null) {
        struct aws_allocator *allocator = aws_default_allocator();
        /* Create a string with non-deterministic content */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        uint8_t *bytes = malloc(len);
        for (size_t i = 0; i < len; i++) {
            bytes[i] = nondet_uint8_t();
        }
        str = aws_string_new_from_array(allocator, bytes, len);
        __CPROVER_assume(str != NULL); /* assume allocation succeeded */
        free(bytes);
        assert(aws_string_is_valid(str));
    }

    /* Non-deterministically decide if buf is NULL */
    bool buf_is_null = nondet_bool();
    struct aws_byte_buf buf;
    struct aws_byte_buf *buf_ptr = NULL;
    struct aws_byte_buf old_buf;
    uint8_t old_buf_contents[MAX_BUFFER_SIZE];
    size_t old_buf_len = 0;
    if (!buf_is_null) {
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_valid(&buf));
        buf_ptr = &buf;
        old_buf = buf; /* save struct for later comparison */
        old_buf_len = buf.len;
        for (size_t i = 0; i < buf.len; i++) {
            old_buf_contents[i] = buf.buffer[i];
        }
    }

    /* Save old string state */
    size_t old_str_len = 0;
    uint8_t *old_str_bytes = NULL;
    if (str != NULL) {
        old_str_len = str->len;
        old_str_bytes = malloc(old_str_len);
        for (size_t i = 0; i < old_str_len; i++) {
            old_str_bytes[i] = str->bytes[i];
        }
    }

    /* Call the function under test */
    bool result = aws_string_eq_byte_buf(str, buf_ptr);

    /* Postconditions: return value correctness */
    if (str == NULL && buf_ptr == NULL) {
        assert(result == true);
    } else if (str == NULL || buf_ptr == NULL) {
        assert(result == false);
    } else {
        /* Both non-NULL: result must match byte equality */
        bool expected;
        if (str->len == buf.len) {
            if (str->len == 0) {
                expected = true;
            } else {
                expected = (memcmp(str->bytes, buf.buffer, str->len) == 0);
            }
        } else {
            expected = false;
        }
        assert(result == expected);
    }

    /* Immutability: string unchanged */
    if (str != NULL) {
        assert(str->len == old_str_len);
        for (size_t i = 0; i < old_str_len; i++) {
            assert(str->bytes[i] == old_str_bytes[i]);
        }
        free(old_str_bytes);
        assert(aws_string_is_valid(str));
    }

    /* Immutability: byte buffer unchanged */
    if (buf_ptr != NULL) {
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        for (size_t i = 0; i < old_buf_len; i++) {
            assert(buf.buffer[i] == old_buf_contents[i]);
        }
        assert(aws_byte_buf_is_valid(&buf));
    }

    /* Clean up */
    if (str != NULL) {
        aws_string_destroy(str);
    }
}
