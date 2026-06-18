#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf buf1;
    struct aws_byte_buf buf2;

    /* Bound and initialize dest */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    dest.len = nondet_size_t();
    __CPROVER_assume(dest.len <= dest.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Bound and initialize buf1 */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    buf1.len = nondet_size_t();
    __CPROVER_assume(buf1.len <= buf1.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));

    /* Bound and initialize buf2 */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf2);
    buf2.len = nondet_size_t();
    __CPROVER_assume(buf2.len <= buf2.capacity);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf2));

    /* Save old states */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_buf1 = buf1;
    struct aws_byte_buf old_buf2 = buf2;
    size_t old_dest_len = dest.len;

    /* Snapshot dest's original content */
    uint8_t old_dest_buffer[MAX_BUFFER_SIZE];
    for (size_t i = 0; i < old_dest_len; i++) {
        old_dest_buffer[i] = dest.buffer[i];
    }

    /* Call the function under verification with 2 source buffers */
    int result = aws_byte_buf_cat(&dest, 2, &buf1, &buf2);

    /* buf1 and buf2 must remain unchanged */
    assert(aws_byte_buf_eq(&buf1, &old_buf1));
    assert(aws_byte_buf_eq(&buf2, &old_buf2));

    /* Unchanged fields of dest */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    if (result == AWS_OP_SUCCESS) {
        /* Both buffers appended successfully */
        assert(dest.len == old_dest_len + buf1.len + buf2.len);

        /* Original bytes unchanged */
        for (size_t i = 0; i < old_dest_len; i++) {
            assert(dest.buffer[i] == old_dest_buffer[i]);
        }
        /* buf1 appended */
        for (size_t i = 0; i < buf1.len; i++) {
            assert(dest.buffer[old_dest_len + i] == buf1.buffer[i]);
        }
        /* buf2 appended after buf1 */
        for (size_t i = 0; i < buf2.len; i++) {
            assert(dest.buffer[old_dest_len + buf1.len + i] == buf2.buffer[i]);
        }
    } else {
        /* Failure: result == AWS_OP_ERR.
         * dest->len reflects bytes copied before the failure.
         */
        if (old_dest_len + buf1.len <= dest.capacity) {
            /* First append succeeded, second failed */
            assert(dest.len == old_dest_len + buf1.len);

            /* Original bytes unchanged */
            for (size_t i = 0; i < old_dest_len; i++) {
                assert(dest.buffer[i] == old_dest_buffer[i]);
            }
            /* buf1 appended */
            for (size_t i = 0; i < buf1.len; i++) {
                assert(dest.buffer[old_dest_len + i] == buf1.buffer[i]);
            }
        } else {
            /* First append failed, dest unchanged */
            assert(dest.len == old_dest_len);

            /* All original bytes unchanged */
            for (size_t i = 0; i < old_dest_len; i++) {
                assert(dest.buffer[i] == old_dest_buffer[i]);
            }
        }
    }

    /* Validity invariants hold for all byte buffers */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&buf1));
    assert(aws_byte_buf_is_valid(&buf2));
}
