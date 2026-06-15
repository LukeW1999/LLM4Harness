#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    struct aws_byte_buf src;

    /* Ensure allocator is not null */
    __CPROVER_assume(allocator != NULL);

    /* Make src valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Ensure dest is uninitialized */
    dest.allocator = NULL;

    /* Ensure src length is valid */
    __CPROVER_assume(src.len <= src.capacity);
    __CPROVER_assume(src.len <= AWS_BYTE_BUF_MAX_SIZE);

    /* Save old state for immutability checks */
    struct aws_byte_buf old_src = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        if (src.buffer != NULL) {
            assert(dest.capacity == src.len);
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
        }
    } else {
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
    }

    /* src is unchanged */
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(aws_byte_buf_is_valid(&src));
}
