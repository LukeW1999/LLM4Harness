#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_string_eq_byte_buf_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    /* 1. Declare nondeterministic inputs */
    struct aws_string *str = NULL;
    struct aws_byte_buf buf;

    /* str may be NULL or a valid string */
    if (nondet_bool()) {
        size_t str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);
        uint8_t *tmp = malloc(str_len);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < str_len; ++i) {
            tmp[i] = nondet_uint8_t();
        }
        str = aws_string_new_from_array(allocator, tmp, str_len);
        free(tmp);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* buf may be NULL (empty) or a valid byte buffer */
    if (nondet_bool()) {
        buf.buffer = NULL;
        buf.len = 0;
        buf.capacity = 0;
        buf.allocator = NULL;
    } else {
        ensure_byte_buf_has_allocated_buffer_member(&buf);
        __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
        size_t buf_len = nondet_size_t();
        __CPROVER_assume(buf_len <= buf.capacity);
        __CPROVER_assume(buf_len <= MAX_BUFFER_SIZE);
        buf.len = buf_len;
        for (size_t i = 0; i < buf.len; ++i) {
            buf.buffer[i] = nondet_uint8_t();
        }
    }

    /* 2. Save old state for immutability checks */
    struct aws_string old_str;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL && str->len > 0) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_buf_bytes;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &old_buf_bytes);
    }

    /* 3. Call function under test */
    bool result = aws_string_eq_byte_buf(str, &buf);

    /* 4. Assert postconditions */
    bool expected = false;
    if (str != NULL && buf.buffer != NULL) {
        if (str->len == buf.len) {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != buf.buffer[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    assert(result == expected);

    /* 5. Unchanged fields */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        if (str->len > 0) {
            assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
        }
    }

    assert(buf.allocator == old_buf.allocator);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    if (buf.buffer != NULL && buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &old_buf_bytes);
    }

    /* 6. Validity invariants */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
