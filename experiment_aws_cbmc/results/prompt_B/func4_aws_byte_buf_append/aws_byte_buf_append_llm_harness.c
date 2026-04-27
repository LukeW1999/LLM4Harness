#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/proof_allocators.h>
#include <proof_helpers/utils.h>

void aws_byte_buf_append_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    /* Bounded sizes */
    size_t max_capacity = nondet_size_t();
    __CPROVER_assume(max_capacity <= MAX_BUFFER_SIZE);
    size_t max_len = nondet_size_t();
    __CPROVER_assume(max_len <= max_capacity);

    /* Allocate and initialize to */
    ensure_aws_byte_buf_has_allocated_buffer_member(&to, max_capacity);
    to.len = nondet_size_t();
    __CPROVER_assume(to.len <= to.capacity);

    /* Allocate and initialize from */
    ensure_aws_byte_cursor_has_allocated_buffer_member(&from, max_len);
    from.len = nondet_size_t();
    __CPROVER_assume(from.len <= from.capacity);

    /* Save old states */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* Preconditions */
    __CPROVER_assume(aws_byte_buf_is_valid(&to));
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
        assert(AWS_MEM_IS_SAME(old_to.buffer, to.buffer, old_to.len));
        assert(AWS_MEM_IS_SAME(old_from.ptr, from.ptr, from.len));
    } else if (result == AWS_OP_ERR) {
        assert(to.len == old_to.len);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
        assert(from.len == old_from.len);
        assert(from.ptr == old_from.ptr);
        assert(AWS_MEM_IS_SAME(old_to.buffer, to.buffer, old_to.len));
        assert(AWS_MEM_IS_SAME(old_from.ptr, from.ptr, from.len));
    }

    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
