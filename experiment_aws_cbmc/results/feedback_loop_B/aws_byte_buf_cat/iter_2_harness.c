#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_cat_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= 5); // Limiting number_of_args for practical bounded verification

    struct aws_byte_buf buffers[number_of_args];
    for (size_t i = 0; i < number_of_args; ++i) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&buffers[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&buffers[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(&buffers[i]));
        __CPROVER_assume(buffers[i].len <= MAX_BUFFER_SIZE - dest.len); // Ensure we do not overflow
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args, buffers);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        size_t expected_len = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            expected_len += buffers[i].len;
        }
        assert(dest.len == expected_len);
    } else {
        assert(dest.len == old_dest.len);
        assert(dest.buffer == old_dest.buffer);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
}
