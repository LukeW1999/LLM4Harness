#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    /* parameters */
    const void *bytes;
    size_t capacity = nondet_size_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    /* allocate bytes non-deterministically */
    bytes = malloc(capacity);
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(bytes, capacity));

    /* call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    /* assertions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.buffer == (capacity > 0) ? (uint8_t *)bytes : NULL);
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);
}
