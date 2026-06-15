#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_c_str_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    char *c_str = (char *)nondet_uint8_t();
    __CPROVER_assume(c_str!= NULL);

    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    if (result) {
        assert(aws_array_eq(buf.buffer, buf.len, c_str, strlen(c_str)));
    } else {
        assert(!aws_array_eq(buf.buffer, buf.len, c_str, strlen(c_str)));
    }

    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    assert(aws_byte_buf_is_valid(&buf));
}
