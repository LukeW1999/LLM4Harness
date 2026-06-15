#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_from_c_str_harness() {
    const char *c_str;
    __CPROVER_assume(c_str == NULL || strlen(c_str) <= MAX_BUFFER_SIZE);

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    if (c_str == NULL) {
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
    } else {
        assert(buf.len == strlen(c_str));
        assert(buf.capacity >= buf.len);
        assert(buf.buffer!= NULL);
        assert(buf.allocator == aws_default_allocator());
    }

    assert(aws_byte_buf_is_valid(&buf));
}
