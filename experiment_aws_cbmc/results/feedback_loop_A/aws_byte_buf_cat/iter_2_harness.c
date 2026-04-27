#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

void aws_byte_buf_cat_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args > 0 && number_of_args <= 5); // Limiting the number of arguments for practical reasons

    struct aws_byte_buf *buffers[number_of_args];
    for (size_t i = 0; i < number_of_args; ++i) {
        buffers[i] = malloc(sizeof(struct aws_byte_buf));
        __CPROVER_assume(buffers[i] != NULL);
        __CPROVER_assume(aws_byte_buf_is_bounded(buffers[i], MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(buffers[i]);
        __CPROVER_assume(aws_byte_buf_is_valid(buffers[i]));
        __CPROVER_assume(buffers[i]->buffer != NULL); // Ensure buffer is not NULL
    }

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_dest = dest;

    /* 3. Call function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args, (struct aws_byte_buf **)&buffers);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        size_t expected_len = old_dest.len;
        for (size_t i = 0; i < number_of_args; ++i) {
            expected_len += buffers[i]->len;
        }
        assert(dest.len == expected_len);
        // Check that the content of dest is the concatenation of all buffers
        size_t pos = 0;
        for (size_t i = 0; i < number_of_args; ++i) {
            for (size_t j = 0; j < buffers[i]->len; ++j) {
                assert(dest.buffer[pos++] == buffers[i]->buffer[j]);
            }
        }
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

    /* Free allocated memory */
    for (size_t i = 0; i < number_of_args; ++i) {
        free(buffers[i]->buffer);
        free(buffers[i]);
    }
    free(dest.buffer);
}
