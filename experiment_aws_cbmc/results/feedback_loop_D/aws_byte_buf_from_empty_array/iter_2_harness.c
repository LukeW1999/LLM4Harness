#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    uint8_t bytes[MAX_BUFFER_SIZE];
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    struct aws_byte_buf old;
    old.buffer = bytes;
    old.len = 0;
    old.capacity = capacity;
    old.allocator = NULL;

    struct aws_byte_buf result = aws_byte_buf_from_empty_array(bytes, capacity);

    // Postconditions
    if (capacity > 0) {
        assert(result.buffer == (uint8_t *)bytes);
    } else {
        assert(result.buffer == NULL);
    }
    assert(result.len == 0);
    assert(result.capacity == capacity);
    assert(result.allocator == NULL);

    // Frame conditions
    assert(result.buffer == (capacity > 0 ? (uint8_t *)bytes : NULL));
    assert(result.len == 0);
    assert(result.capacity == capacity);
    assert(result.allocator == NULL);

    // Validity invariants
    assert(aws_byte_buf_is_valid(&result));
}
