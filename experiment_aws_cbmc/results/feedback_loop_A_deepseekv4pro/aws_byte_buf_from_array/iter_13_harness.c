#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1000

void aws_byte_buf_from_array_harness() {
    uint8_t data[MAX_BUFFER_SIZE];
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    const uint8_t *array_ptr = data;

    struct aws_byte_buf buf = aws_byte_buf_from_array(array_ptr, len);

    /* The returned byte_buf has the expected length and capacity, and no allocator */
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* If the length is non-zero, the buffer must be non-NULL and readable */
    if (len > 0) {
        assert(buf.buffer != NULL);
        __CPROVER_assert(__CPROVER_r_ok(buf.buffer, len), "buffer is readable");
    }

    assert(aws_byte_buf_is_valid(&buf));
}
