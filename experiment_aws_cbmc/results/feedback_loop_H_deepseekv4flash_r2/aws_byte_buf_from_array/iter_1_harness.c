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
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministic inputs */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;

    /* ensure preconditions */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* bound the size of src to avoid excessive analysis */
    size_t max_capacity = nondet_size_t();
    __CPROVER_assume(max_capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(src.capacity <= max_capacity);
    __CPROVER_assume(src.len <= src.capacity);

    /* ensure src.buffer is allocated if capacity > 0 */
    if (src.capacity > 0) {
        ensure_byte_buf_has_allocated_buffer_member(&src);
        __CPROVER_assume(AWS_MEM_IS_WRITABLE(src.buffer, src.capacity));
    } else {
        __CPROVER_assume(src.buffer == NULL);
    }

    /* save old state of src for immutability check */
    struct aws_byte_buf old_src = src;

    /* call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest fields */
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);
        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* src.buffer was NULL → dest should be zeroed */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        }
    } else {
        /* on failure, dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
    }

    /* src must remain unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.capacity > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    }

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src));
}
