#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf buf;
    size_t len = nondet_size_t();
    uint8_t *bytes = (uint8_t *)can_fail_malloc(len);

    __CPROVER_assume(bytes != NULL || len == 0);

    aws_byte_buf_from_array(&buf, bytes, len);

    // Step 1: Success Path Assertions
    if (len > 0) {
        assert(buf.buffer == bytes);
    } else {
        assert(buf.buffer == NULL);
    }
    assert(buf.len == len);
    assert(buf.capacity == len);
    assert(buf.allocator == NULL);

    // Step 4: Validity Invariants Assertions
    assert(aws_byte_buf_is_valid(&buf));
}
