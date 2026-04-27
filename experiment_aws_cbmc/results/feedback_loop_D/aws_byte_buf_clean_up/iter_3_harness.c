#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_clean_up_harness() {
    struct aws_byte_buf buf;

    // Initialize buf with allocated buffer
    make_and_initialize_byte_buf(&buf, sizeof(uint8_t) * 10);

    // Assume buf is valid
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    // Call the function
    aws_byte_buf_clean_up(&buf);

    // Postcondition assertions
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
}
