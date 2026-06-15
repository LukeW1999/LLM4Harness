#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_from_c_str_harness() {
    const char *c_str = (const char *)nondet_uint8_t();
    __CPROVER_assume(c_str != NULL);

    struct aws_byte_buf buf = aws_byte_buf_from_c_str(c_str);

    assert(buf.len == strlen(c_str));
    assert(buf.capacity == buf.len);
    assert(buf.buffer == (uint8_t *)c_str);
    assert(buf.allocator == NULL);

    assert(aws_byte_buf_is_valid(&buf));
}
