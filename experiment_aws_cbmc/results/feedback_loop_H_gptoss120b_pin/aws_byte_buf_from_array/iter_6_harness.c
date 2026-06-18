#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_from_array_harness(void) {
    struct aws_byte_buf buf;
    const uint8_t *bytes;
    size_t len;

    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    uint8_t data[MAX_BUFFER_SIZE];

    if (len > 0) {
        bytes = data;
    } else {
        bytes = NULL;
    }

    aws_byte_buf_from_array(&buf, bytes, len);

    assert(buf.allocator == NULL);
    assert(buf.buffer == (uint8_t *)bytes);
    assert(buf.len == len);
    assert(buf.capacity == len);

    if (bytes == NULL) {
        assert(len == 0);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
