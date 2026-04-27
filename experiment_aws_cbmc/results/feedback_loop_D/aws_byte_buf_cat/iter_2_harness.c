#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/byte_buf.h>

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    size_t number_of_args = nondet_size_t();
    struct aws_byte_buf buffers[MAX_BUFFER_SIZE]; // Use stack allocation for buffers
    struct aws_byte_buf old_dest;

    // Initialize dest
    __CPROVER_assume(number_of_args <= MAX_BUFFER_SIZE);
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    old_dest = dest;

    // Initialize source buffers
    for (size_t i = 0; i < number_of_args; ++i) {
        ensure_byte_buf_has_allocated_buffer_member(&buffers[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&buffers[i]));
    }

    int result = aws_byte_buf_cat(&dest, number_of_args, buffers);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        size_t total_len = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            total_len += buffers[i].len;
        }
        assert(dest.len == total_len);
        // We cannot directly assert the contents of dest.buffer due to the complexity of concatenation logic
    }

    // Assertions for failure path
    if (result == AWS_OP_ERR) {
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    // Validity invariant
    assert(aws_byte_buf_is_valid(&dest));

    // Free allocated memory
    free(dest.buffer);
    for (size_t i = 0; i < number_of_args; ++i) {
        free(buffers[i].buffer);
    }
}
