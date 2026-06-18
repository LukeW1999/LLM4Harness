#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_harness() {
    /* Declare structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Initialize src with valid, bounded data */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src for immutability check */
    struct aws_byte_buf old_src = src;
    uint8_t *old_src_buffer = src.buffer;
    size_t old_src_len = src.len;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));
        /* dest allocator must be the passed allocator */
        assert(dest.allocator == allocator);
        /* dest len and capacity must equal src len and capacity */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        /* If src buffer was NULL, dest buffer must be NULL and len/capacity 0 */
        if (old_src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest buffer must be non-NULL and different from src buffer */
            assert(dest.buffer != NULL);
            assert(dest.buffer != old_src.buffer);
            /* Contents must match */
            assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
        }
    } else {
        /* Failure: dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* dest should still be valid (zeroed struct is valid) */
        assert(aws_byte_buf_is_valid(&dest));
    }

    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    if (old_src.buffer != NULL) {
        assert_bytes_match(src.buffer, old_src.buffer, old_src.len);
    }
}
