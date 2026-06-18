#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness(void) {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    struct aws_byte_buf src1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    struct aws_byte_buf src2;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* 2. Additional assumptions to avoid overlapping buffers and ensure capacity */
    __CPROVER_assume(dest.buffer != src1.buffer);
    __CPROVER_assume(dest.buffer != src2.buffer);
    __CPROVER_assume(src1.buffer != src2.buffer);
    __CPROVER_assume(dest.capacity >= dest.len + src1.len + src2.len);
    __CPROVER_assume(src1.len == 0 || src1.buffer != NULL);
    __CPROVER_assume(src2.len == 0 || src2.buffer != NULL);

    /* 3. Save old state BEFORE calling */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    struct store_byte_from_buffer src1_storage = {0};
    struct store_byte_from_buffer src2_storage = {0};

    if (src1.len > 0) {
        save_byte_from_array(src1.buffer, src1.len, &src1_storage);
    }
    if (src2.len > 0) {
        save_byte_from_array(src2.buffer, src2.len, &src2_storage);
    }

    /* 4. Call function under test */
    int result = aws_byte_buf_cat(&dest, 2, &src1, &src2);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest length increased by sum of source lengths */
        assert(dest.len == old_dest.len + src1.len + src2.len);

        /* dest capacity, allocator, and buffer pointer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);

        /* original part of dest unchanged */
        assert_bytes_match(dest.buffer, old_dest.buffer, old_dest.len);

        /* appended part matches src1 then src2 */
        assert_bytes_match(dest.buffer + old_dest.len, src1.buffer, src1.len);
        assert_bytes_match(dest.buffer + old_dest.len + src1.len, src2.buffer, src2.len);
    } else {
        /* on error, only require dest to remain a valid byte buffer */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src buffers must remain unchanged regardless of result */
    assert(src1.len == old_src1.len);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);
    assert(src1.buffer == old_src1.buffer);
    if (src1.len > 0) {
        assert_byte_from_buffer_matches(src1.buffer, &src1_storage);
    }

    assert(src2.len == old_src2.len);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.allocator == old_src2.allocator);
    assert(src2.buffer == old_src2.buffer);
    if (src2.len > 0) {
        assert_byte_from_buffer_matches(src2.buffer, &src2_storage);
    }

    /* 6. Invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
}
