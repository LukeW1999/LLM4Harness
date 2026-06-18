#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

static void assert_bytes_match_if_nonzero(const uint8_t *a, const uint8_t *b, size_t len) {
    if (len > 0) {
        assert_bytes_match(a, b, len);
    }
}

static void assert_cat_bytes_match(
    const struct aws_byte_buf *dest,
    size_t dest_offset,
    const struct aws_byte_buf *src0,
    const struct aws_byte_buf *src1,
    const struct aws_byte_buf *src2,
    size_t number_of_args,
    size_t bytes_to_check) {

    size_t remaining = bytes_to_check;
    size_t offset = dest_offset;

    size_t src0_bytes = remaining < src0->len ? remaining : src0->len;
    assert_bytes_match_if_nonzero(dest->buffer + offset, src0->buffer, src0_bytes);
    offset += src0_bytes;
    remaining -= src0_bytes;

    size_t src1_bytes = remaining < src1->len ? remaining : src1->len;
    assert_bytes_match_if_nonzero(dest->buffer + offset, src1->buffer, src1_bytes);
    offset += src1_bytes;
    remaining -= src1_bytes;

    if (number_of_args == 3) {
        size_t src2_bytes = remaining < src2->len ? remaining : src2->len;
        assert_bytes_match_if_nonzero(dest->buffer + offset, src2->buffer, src2_bytes);
        offset += src2_bytes;
        remaining -= src2_bytes;
    }

    assert(remaining == 0);
}

void aws_byte_buf_cat_harness(void) {
    struct aws_byte_buf dest;
    struct aws_byte_buf src0;
    struct aws_byte_buf src1;
    struct aws_byte_buf src2;

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&dest);
    ensure_byte_buf_has_allocated_buffer_member(&src0);
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    ensure_byte_buf_has_allocated_buffer_member(&src2);

    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_valid(&src0));
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    if (dest.capacity > 0 && src0.capacity > 0) {
        __CPROVER_assume(__CPROVER_separate(dest.buffer, src0.buffer));
    }
    if (dest.capacity > 0 && src1.capacity > 0) {
        __CPROVER_assume(__CPROVER_separate(dest.buffer, src1.buffer));
    }
    if (dest.capacity > 0 && src2.capacity > 0) {
        __CPROVER_assume(__CPROVER_separate(dest.buffer, src2.buffer));
    }

    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args == 2 || number_of_args == 3);

    __CPROVER_assume(src0.len <= SIZE_MAX - src1.len);
    size_t first_two_len = src0.len + src1.len;
    if (number_of_args == 3) {
        __CPROVER_assume(first_two_len <= SIZE_MAX - src2.len);
    }

    size_t total_source_len = first_two_len;
    if (number_of_args == 3) {
        total_source_len += src2.len;
    }

    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src0 = src0;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    struct store_byte_from_buffer old_dest_byte;
    struct store_byte_from_buffer old_src0_byte;
    struct store_byte_from_buffer old_src1_byte;
    struct store_byte_from_buffer old_src2_byte;

    save_byte_from_array(dest.buffer, dest.len, &old_dest_byte);
    save_byte_from_array(src0.buffer, src0.len, &old_src0_byte);
    save_byte_from_array(src1.buffer, src1.len, &old_src1_byte);
    save_byte_from_array(src2.buffer, src2.len, &old_src2_byte);

    size_t old_available = old_dest.capacity - old_dest.len;

    int result = aws_byte_buf_cat(&dest, number_of_args, &src0, &src1, &src2);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    assert(src0.len == old_src0.len);
    assert(src0.buffer == old_src0.buffer);
    assert(src0.capacity == old_src0.capacity);
    assert(src0.allocator == old_src0.allocator);

    assert(src1.len == old_src1.len);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);

    assert(src2.len == old_src2.len);
    assert(src2.buffer == old_src2.buffer);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.allocator == old_src2.allocator);

    assert_byte_from_buffer_matches(dest.buffer, &old_dest_byte);
    assert_byte_from_buffer_matches(src0.buffer, &old_src0_byte);
    assert_byte_from_buffer_matches(src1.buffer, &old_src1_byte);
    assert_byte_from_buffer_matches(src2.buffer, &old_src2_byte);

    if (result == AWS_OP_SUCCESS) {
        assert(total_source_len <= old_available);
        assert(dest.len == old_dest.len + total_source_len);
        assert_cat_bytes_match(&dest, old_dest.len, &old_src0, &old_src1, &old_src2, number_of_args, total_source_len);
    } else {
        assert(total_source_len > old_available);
        assert(dest.len == old_dest.len + old_available);
        assert(dest.len == old_dest.capacity);
        assert_cat_bytes_match(&dest, old_dest.len, &old_src0, &old_src1, &old_src2, number_of_args, old_available);
    }

    if (total_source_len <= old_available) {
        assert(result == AWS_OP_SUCCESS);
    } else {
        assert(result == AWS_OP_ERR);
    }

    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src0));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
}
