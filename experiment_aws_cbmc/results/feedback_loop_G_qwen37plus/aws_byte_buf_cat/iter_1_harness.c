#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness(void) {
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    struct aws_byte_buf old_dest = dest;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(dest.buffer, dest.capacity, &old_byte);

    size_t number_of_args;
    __CPROVER_assume(number_of_args <= 2);

    struct aws_byte_buf buf1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));

    struct aws_byte_buf buf2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf2);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));

    int result;
    if (number_of_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (number_of_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, &buf1);
    } else {
        result = aws_byte_buf_cat(&dest, 2, &buf1, &buf2);
    }

    if (result == AWS_OP_SUCCESS) {
        if (number_of_args == 0) {
            assert(dest.len == old_dest.len);
        } else if (number_of_args == 1) {
            assert(dest.len == old_dest.len + buf1.len);
        } else {
            assert(dest.len == old_dest.len + buf1.len + buf2.len);
        }
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        assert_byte_from_buffer_matches(dest.buffer, &old_byte);
        assert(aws_byte_buf_is_valid(&dest));
    }
}
