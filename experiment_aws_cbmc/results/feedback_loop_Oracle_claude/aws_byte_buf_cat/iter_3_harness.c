#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 8

void aws_byte_buf_cat_harness(void) {
    struct aws_byte_buf dest;
    struct aws_byte_buf buffer1;
    struct aws_byte_buf buffer2;
    struct aws_byte_buf buffer3;

    /* Setup source buffers with bounded, valid state */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer1));

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer2);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer2));

    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer3, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer3);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer3));

    /* Setup dest buffer with bounded, valid state */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Ensure no overlap between dest buffer and source buffers */
    __CPROVER_assume(dest.buffer != buffer1.buffer);
    __CPROVER_assume(dest.buffer != buffer2.buffer);
    __CPROVER_assume(dest.buffer != buffer3.buffer);
    __CPROVER_assume(buffer1.buffer != buffer2.buffer);
    __CPROVER_assume(buffer1.buffer != buffer3.buffer);
    __CPROVER_assume(buffer2.buffer != buffer3.buffer);

    /* Save pre-call state for frame condition checks */
    size_t old_dest_capacity = dest.capacity;
    size_t old_dest_len = dest.len;
    uint8_t *old_dest_buffer = dest.buffer;
    struct aws_allocator *old_dest_allocator = dest.allocator;

    size_t old_buf1_len = buffer1.len;
    size_t old_buf1_capacity = buffer1.capacity;
    uint8_t *old_buf1_buffer = buffer1.buffer;

    size_t old_buf2_len = buffer2.len;
    size_t old_buf2_capacity = buffer2.capacity;
    uint8_t *old_buf2_buffer = buffer2.buffer;

    size_t old_buf3_len = buffer3.len;
    size_t old_buf3_capacity = buffer3.capacity;
    uint8_t *old_buf3_buffer = buffer3.buffer;

    /* Call the function under verification */
    int result = aws_byte_buf_cat(&dest, 3, &buffer1, &buffer2, &buffer3);

    /* Postcondition 1: Return value correctness */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: Output buffer validity */
    assert(aws_byte_buf_is_valid(&dest));

    /* Postcondition 3: Capacity is never decreased */
    assert(dest.capacity >= old_dest_capacity);

    /* Postcondition 4: Buffer pointer and allocator are unchanged (no reallocation in cat) */
    assert(dest.buffer == old_dest_buffer);
    assert(dest.allocator == old_dest_allocator);

    /* Postcondition 5: Length invariants */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_dest_len + buffer1.len + buffer2.len + buffer3.len);
        assert(dest.len <= dest.capacity);
    } else {
        assert(dest.len <= dest.capacity);
        assert(dest.len >= old_dest_len);
    }

    /* Postcondition 6: Frame conditions - source buffers are not modified */
    assert(buffer1.len == old_buf1_len);
    assert(buffer1.capacity == old_buf1_capacity);
    assert(buffer1.buffer == old_buf1_buffer);

    assert(buffer2.len == old_buf2_len);
    assert(buffer2.capacity == old_buf2_capacity);
    assert(buffer2.buffer == old_buf2_buffer);

    assert(buffer3.len == old_buf3_len);
    assert(buffer3.capacity == old_buf3_capacity);
    assert(buffer3.buffer == old_buf3_buffer);

    /* Postcondition 7: Source buffers remain valid */
    assert(aws_byte_buf_is_valid(&buffer1));
    assert(aws_byte_buf_is_valid(&buffer2));
    assert(aws_byte_buf_is_valid(&buffer3));
}
