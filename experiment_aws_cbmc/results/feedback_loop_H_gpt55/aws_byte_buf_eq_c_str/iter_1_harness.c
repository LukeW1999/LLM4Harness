#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len < SIZE_MAX);

    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < c_str_len) {
            uint8_t ch = nondet_uint8_t();
            __CPROVER_assume(ch != 0);
            c_str[i] = (char)ch;
        }
    }
    c_str[c_str_len] = '\0';

    struct aws_byte_buf old_buf = buf;

    struct store_byte_from_buffer old_buf_byte;
    if (buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &old_buf_byte);
    }

    struct store_byte_from_buffer old_c_str_byte;
    save_byte_from_array((const uint8_t *)c_str, c_str_len + 1, &old_c_str_byte);

    bool expected = (buf.len == c_str_len);
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < buf.len && i < c_str_len) {
            if (buf.buffer[i] != (uint8_t)c_str[i]) {
                expected = false;
            }
        }
    }

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    assert(result == expected);

    if (result) {
        assert(buf.len == c_str_len);
        for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
            if (i < buf.len) {
                assert(buf.buffer[i] == (uint8_t)c_str[i]);
                assert(buf.buffer[i] != 0);
            }
        }
    } else {
        assert(!expected);
        if (buf.len == c_str_len) {
            bool mismatch_found = false;
            for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
                if (i < buf.len) {
                    if (buf.buffer[i] != (uint8_t)c_str[i]) {
                        mismatch_found = true;
                    }
                }
            }
            assert(mismatch_found);
        } else {
            assert(buf.len != c_str_len);
        }
    }

    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    if (buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &old_buf_byte);
    }

    assert(c_str[c_str_len] == '\0');
    for (size_t i = 0; i < MAX_BUFFER_SIZE; ++i) {
        if (i < c_str_len) {
            assert(c_str[i] != '\0');
        }
    }
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_byte);

    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
}
