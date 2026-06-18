#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void __CPROVER_havoc_object(void *);

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str_len < SIZE_MAX);

    size_t c_str_alloc_len = c_str_len + 1;
    char *c_str = malloc(c_str_alloc_len);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(c_str, c_str_alloc_len));

    __CPROVER_havoc_object(c_str);
    c_str[c_str_len] = '\0';

    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_alloc_len));

    struct aws_byte_buf old = buf;

    struct store_byte_from_buffer old_buf_byte;
    save_byte_from_array(buf.buffer, buf.capacity, &old_buf_byte);

    struct store_byte_from_buffer old_c_str_byte;
    save_byte_from_array((const uint8_t *)c_str, c_str_alloc_len, &old_c_str_byte);

    size_t actual_c_str_len = strlen(c_str);
    bool expected = false;

    if (old.len == actual_c_str_len) {
        if (old.len == 0) {
            expected = true;
        } else {
            expected = memcmp(old.buffer, c_str, old.len) == 0;
        }
    }

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    if (result) {
        assert(expected);
        assert(buf.len == old.len);
        assert(buf.buffer == old.buffer);
        assert(buf.capacity == old.capacity);
        assert(buf.allocator == old.allocator);

        if (old.len > 0) {
            size_t i = nondet_size_t();
            __CPROVER_assume(i < old.len);
            assert(old.buffer[i] != 0);
        }
    } else {
        assert(!expected);
        assert(buf.len == old.len);
        assert(buf.buffer == old.buffer);
        assert(buf.capacity == old.capacity);
        assert(buf.allocator == old.allocator);
    }

    assert(result == expected);

    assert(buf.len == old.len);
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    assert_byte_from_buffer_matches(buf.buffer, &old_buf_byte);
    assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_byte);

    assert(aws_byte_buf_is_valid(&buf));
}
