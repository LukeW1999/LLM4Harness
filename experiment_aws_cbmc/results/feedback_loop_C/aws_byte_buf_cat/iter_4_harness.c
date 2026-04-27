#include <aws/common/byte_buf.h>
#include <stddef.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_cat_harness() {
    struct aws_byte_buf dest;
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args > 0 && number_of_args <= MAX_BUFFER_SIZE / sizeof(struct aws_byte_buf *));
    
    // Initialize dest with a bounded buffer
    size_t dest_capacity = nondet_size_t();
    __CPROVER_assume(dest_capacity <= MAX_BUFFER_SIZE);
    dest.buffer = bounded_malloc(dest_capacity);
    dest.len = nondet_size_t();
    __CPROVER_assume(dest.len <= dest_capacity);
    dest.capacity = dest_capacity;
    struct aws_allocator *allocator = aws_default_allocator();
    dest.allocator = allocator;

    // Create an array of source byte buffers
    struct aws_byte_buf sources[number_of_args];
    for (size_t i = 0; i < number_of_args; ++i) {
        size_t source_capacity = nondet_size_t();
        __CPROVER_assume(source_capacity <= MAX_BUFFER_SIZE - dest.len); // Ensure we don't overflow dest
        sources[i].buffer = bounded_malloc(source_capacity);
        sources[i].len = nondet_size_t();
        __CPROVER_assume(sources[i].len <= source_capacity);
        sources[i].capacity = source_capacity;
        sources[i].allocator = allocator;
    }

    // Save old state of dest
    struct aws_byte_buf old_dest = dest;

    // Ensure dest and sources are valid before calling aws_byte_buf_cat
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    for (size_t i = 0; i < number_of_args; ++i) {
        __CPROVER_assume(aws_byte_buf_is_valid(&sources[i]));
    }

    // Call the function under test
    int result = aws_byte_buf_cat(&dest, number_of_args, sources);

    // Assertions for success path
    if (result == AWS_OP_SUCCESS) {
        // Check that dest.len is the sum of lengths of all source buffers
        size_t total_len = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            total_len += sources[i].len;
        }
        __CPROVER_assume(total_len <= dest_capacity); // Ensure dest can hold the concatenated result
        assert(dest.len == total_len);

        // Check that dest.buffer contains concatenated contents of all source buffers
        size_t offset = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            assert_bytes_match(dest.buffer + offset, sources[i].buffer, sources[i].len);
            offset += sources[i].len;
        }
    } else {
        // Assertions for failure path
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    // Check validity of dest
    assert(aws_byte_buf_is_valid(&dest));

    // Free allocated memory
    for (size_t i = 0; i < number_of_args; ++i) {
        if (sources[i].buffer) {
            free(sources[i].buffer);
        }
    }
    if (dest.buffer) {
        free(dest.buffer);
    }
}
