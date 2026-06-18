#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1000

void aws_byte_buf_from_array_harness() {
    /* A concrete array to provide a valid, bounded memory region */
    uint8_t data[MAX_BUFFER_SIZE];
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    const uint8_t *array_ptr = data;

    struct aws_byte_buf buf = aws_byte_buf_from_array(array_ptr, len);

    /* Postconditions: the returned byte_buf wraps the original array */
    assert(buf.buffer == array_ptr);
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    /* The structure should pass the library’s validity check */
    assert(aws_byte_buf_is_valid(&buf));

    /* The original array contents must not be modified (no explicit action needed) */
}
