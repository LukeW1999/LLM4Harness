#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_harness() {
    /* data structure */
    struct aws_byte_buf buf; /* Precondition: buf is non-null */

    /* parameters */
    struct aws_allocator *allocator = aws_default_allocator(); /* Precondition: allocator is non-null */
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* Save old state (for checking immutability) */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buf.buffer, buf.capacity, &old_byte);

    /* Call function under test */
    int result = aws_byte_buf_init(&buf, allocator, capacity);

    /* Assert postconditions (both branches) */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        assert(buf.buffer != NULL || capacity == 0);
        assert(AWS_MEM_IS_WRITABLE(buf.buffer, buf.capacity));
    } else {
        assert(result == AWS_OP_ERR);
        assert(buf.buffer == NULL);
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.allocator == NULL);
    }
    assert(aws_byte_buf_is_valid(&buf));  // invariant always holds
    assert_byte_from_buffer_matches(buf.buffer, &old_byte);  // if read-only
}
