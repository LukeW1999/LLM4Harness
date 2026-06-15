#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE ((size_t)1024)

void aws_byte_buf_init_from_empty_array_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    size_t capacity = (size_t)nondet_uint64_t();
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);

    int result = aws_byte_buf_init_from_empty_array(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        if (capacity == 0) {
            assert(buf.buffer == NULL);
        } else {
            assert(buf.buffer != NULL);
        }
    } else {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == allocator);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
