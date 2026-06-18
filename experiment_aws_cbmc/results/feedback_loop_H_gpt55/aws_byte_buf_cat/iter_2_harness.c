#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

int nondet_int(void);

static void assert_bytes_match(const uint8_t *a, const uint8_t *b, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        assert(a[i] == b[i]);
    }
}

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src1;
    struct aws_byte_buf src2;
    struct aws_byte_buf src3;

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src3, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&dest);
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    ensure_byte_buf_has_allocated_buffer_member(&src3);

    dest.allocator = aws_default_allocator();
    src1.allocator = aws_default_allocator();
    src2.allocator = aws_default_allocator();
    src3.allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));
    __CPROVER_assume(aws_byte_buf_is_valid(&src3));

    __CPROVER_assume(src1.len <= SIZE_MAX - src2.len);
    size_t total_two = src1.len + src2.len;
    __CPROVER_assume(src3.len <= SIZE_MAX - total_two);
    size_t total_three = total_two + src3.len;

    int use_three_args = nondet_int();
    __CPROVER_assume(use_three_args == 0 || use_three_args == 1);

    size_t number_of_args = use_three_args ? 3 : 2;
    assert(number_of_args > 1);

    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;
    struct aws_byte_buf old_src3 = src3;

    int result;
    if (use_three_args) {
        result = aws_byte_buf_cat(&dest, number_of_args, &src1, &src2, &src3);
    } else {
        result = aws_byte_buf_cat(&dest, number_of_args, &src1, &src2);
    }

    size_t total_to_copy = use_three_args ? total_three : total_two;
    size_t available = old_dest.capacity - old_dest.len;

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(total_to_copy <= available);
        assert(dest.len == old_dest.len + total_to_copy);

        size_t offset = old_dest.len;

        if (old_src1.len > 0) {
            assert_bytes_match(dest.buffer + offset, old_src1.buffer, old_src1.len);
        }
        offset += old_src1.len;

        if (old_src2.len > 0) {
            assert_bytes_match(dest.buffer + offset, old_src2.buffer, old_src2.len);
        }
        offset += old_src2.len;

        if (use_three_args && old_src3.len > 0) {
            assert_bytes_match(dest.buffer + offset, old_src3.buffer, old_src3.len);
        }
    } else {
        assert(total_to_copy > available);

        size_t offset = old_dest.len;

        if (old_src1.len > available) {
            assert(dest.len == old_dest.len);
        } else {
            assert(dest.len >= old_dest.len + old_src1.len);

            if (old_src1.len > 0) {
                assert_bytes_match(dest.buffer + offset, old_src1.buffer, old_src1.len);
            }
            offset += old_src1.len;

            if (old_src2.len > available - old_src1.len) {
                assert(dest.len == old_dest.len + old_src1.len);
            } else {
                assert(use_three_args);
                assert(dest.len == old_dest.len + old_src1.len + old_src2.len);

                if (old_src2.len > 0) {
                    assert_bytes_match(dest.buffer + offset, old_src2.buffer, old_src2.len);
                }
            }
        }
    }

    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    assert(src1.len == old_src1.len);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);

    assert(src2.len == old_src2.len);
    assert(src2.buffer == old_src2.buffer);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.allocator == old_src2.allocator);

    assert(src3.len == old_src3.len);
    assert(src3.buffer == old_src3.buffer);
    assert(src3.capacity == old_src3.capacity);
    assert(src3.allocator == old_src3.allocator);

    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
    assert(aws_byte_buf_is_valid(&src3));
}
