#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_CAT_BUFFER_SIZE 16

static uint8_t s_expected_cat_byte(
    size_t offset,
    const uint8_t *src1,
    size_t src1_len,
    const uint8_t *src2,
    size_t src2_len,
    const uint8_t *src3,
    size_t src3_len,
    size_t number_of_args) {

    if (offset < src1_len) {
        return src1[offset];
    }

    offset -= src1_len;

    if (offset < src2_len) {
        return src2[offset];
    }

    offset -= src2_len;

    if (number_of_args == 3 && offset < src3_len) {
        return src3[offset];
    }

    return 0;
}

void aws_byte_buf_cat_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    size_t dest_capacity;
    __CPROVER_assume(dest_capacity <= MAX_CAT_BUFFER_SIZE);

    struct aws_byte_buf dest;
    if (aws_byte_buf_init(&dest, allocator, dest_capacity) != AWS_OP_SUCCESS) {
        return;
    }

    size_t dest_len;
    __CPROVER_assume(dest_len <= dest.capacity);
    dest.len = dest_len;

    uint8_t src1_storage[MAX_CAT_BUFFER_SIZE];
    uint8_t src2_storage[MAX_CAT_BUFFER_SIZE];
    uint8_t src3_storage[MAX_CAT_BUFFER_SIZE];

    size_t src1_len;
    size_t src2_len;
    size_t src3_len;
    size_t number_of_args;

    __CPROVER_assume(src1_len <= MAX_CAT_BUFFER_SIZE);
    __CPROVER_assume(src2_len <= MAX_CAT_BUFFER_SIZE);
    __CPROVER_assume(src3_len <= MAX_CAT_BUFFER_SIZE);
    __CPROVER_assume(number_of_args == 2 || number_of_args == 3);

    struct aws_byte_cursor src1 = {
        .len = src1_len,
        .ptr = src1_storage,
    };

    struct aws_byte_cursor src2 = {
        .len = src2_len,
        .ptr = src2_storage,
    };

    struct aws_byte_cursor src3 = {
        .len = src3_len,
        .ptr = src3_storage,
    };

    size_t old_dest_len = dest.len;
    size_t old_dest_capacity = dest.capacity;
    uint8_t old_dest_bytes[MAX_CAT_BUFFER_SIZE];

    for (size_t i = 0; i < MAX_CAT_BUFFER_SIZE; ++i) {
        if (i < old_dest_len) {
            old_dest_bytes[i] = dest.buffer[i];
        } else {
            old_dest_bytes[i] = 0;
        }
    }

    size_t expected_len = old_dest_len + src1.len + src2.len;
    if (number_of_args == 3) {
        expected_len += src3.len;
    }

    int result;
    if (number_of_args == 2) {
        result = aws_byte_buf_cat(&dest, 2, &src1, &src2);
    } else {
        result = aws_byte_buf_cat(&dest, 3, &src1, &src2, &src3);
    }

    if (expected_len <= old_dest_capacity) {
        assert(result == AWS_OP_SUCCESS);
        assert(dest.len == expected_len);
        assert(dest.capacity == old_dest_capacity);
        assert(dest.allocator == allocator);

        for (size_t i = 0; i < MAX_CAT_BUFFER_SIZE; ++i) {
            if (i < old_dest_len) {
                assert(dest.buffer[i] == old_dest_bytes[i]);
            }

            if (i >= old_dest_len && i < expected_len) {
                assert(dest.buffer[i] == s_expected_cat_byte(
                                             i - old_dest_len,
                                             src1.ptr,
                                             src1.len,
                                             src2.ptr,
                                             src2.len,
                                             src3.ptr,
                                             src3.len,
                                             number_of_args));
            }
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(dest.len == old_dest_len);
        assert(dest.capacity == old_dest_capacity);
        assert(dest.allocator == allocator);

        for (size_t i = 0; i < MAX_CAT_BUFFER_SIZE; ++i) {
            if (i < old_dest_len) {
                assert(dest.buffer[i] == old_dest_bytes[i]);
            }
        }
    }

    aws_byte_buf_clean_up(&dest);
}
