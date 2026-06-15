#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_advance_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf buffer;
    struct aws_byte_buf output;
    size_t len;

    /* Preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));
    /* output is a local variable, so its pointer is valid; ensure writing is safe */
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(&output, sizeof(struct aws_byte_buf)));

    /* Save old state */
    struct aws_byte_buf old_buffer = buffer;
    uint8_t *old_buffer_ptr = buffer.buffer; /* we will check pointer unchanged */
    size_t old_len = buffer.len;
    size_t old_capacity = buffer.capacity;
    struct aws_allocator *old_allocator = buffer.allocator;

    /* Call the function */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    /* Postcondition: buffer always remains valid */
    assert(aws_byte_buf_is_valid(&buffer));
    /* Postcondition: output always remains valid */
    assert(aws_byte_buf_is_valid(&output));

    if (result) {
        /* Success path */
        /* buffer->len increases by len */
        assert(buffer.len == old_len + len);
        /* buffer->capacity unchanged */
        assert(buffer.capacity == old_capacity);
        /* buffer->allocator unchanged */
        assert(buffer.allocator == old_allocator);
        /* buffer->buffer pointer unchanged */
        assert(buffer.buffer == old_buffer_ptr);

        /* output properties */
        assert(output.len == 0);
        assert(output.capacity == len);
        assert(output.allocator == NULL);
        /* output->buffer points to old buffer + old_len, or NULL if buffer->buffer was NULL */
        if (old_buffer_ptr == NULL) {
            assert(output.buffer == NULL);
        } else {
            assert(output.buffer == old_buffer_ptr + old_len);
            /* The output buffer is readable (it points inside the original buffer) */
            assert(AWS_MEM_IS_READABLE(output.buffer, output.capacity));
        }
    } else {
        /* Failure path */
        /* buffer unchanged */
        assert(buffer.len == old_len);
        assert(buffer.capacity == old_capacity);
        assert(buffer.allocator == old_allocator);
        assert(buffer.buffer == old_buffer_ptr);

        /* output is zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);
    }
}
